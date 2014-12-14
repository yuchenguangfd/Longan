/*
 * svd_train.cpp
 * Created on: Dec 13, 2014
 * Author: chenguangyu
 */

#include "svd_train.h"
#include <string>
#include <vector>
#include <memory>
#include <limits>
#include <condition_variable>
#include <queue>
#include <algorithm>
#include <thread>
#include <iostream>
#include <cstring>
#include <cassert>
#include <cstdlib>

namespace longan {

Timer::Timer()
{
    reset();
}

void Timer::reset()
{
    begin = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>
                   (begin-begin);
}

void Timer::reset(std::string const &msg)
{
    printf("%s", msg.c_str());
    fflush(stdout);
    reset();
}

void Timer::tic()
{
    begin = std::chrono::high_resolution_clock::now();
}

void Timer::tic(std::string const &msg)
{
    printf("%s", msg.c_str());
    fflush(stdout);
    tic();
}

float Timer::toc()
{
    duration += std::chrono::duration_cast<std::chrono::milliseconds>
                    (std::chrono::high_resolution_clock::now()-begin);
    return (double)duration.count()/1000;
}

float Timer::toc(std::string const &msg)
{
    float duration_one = toc();
    printf("%s  %.2f\n", msg.c_str(), duration_one);
    fflush(stdout);
    return duration_one;
}

std::shared_ptr<Matrix> read_matrix_meta(FILE *f)
{
    std::shared_ptr<Matrix> M(new Matrix);
    fread(&M->nr_users, sizeof(int), 1, f);
    fread(&M->nr_items, sizeof(int), 1, f);
    fread(&M->nr_ratings, sizeof(long), 1, f);
    fread(&M->avg, sizeof(float), 1, f);
    return M;
}

std::shared_ptr<Matrix> read_matrix_meta(std::string const &path)
{
    FILE *f = fopen(path.c_str(), "rb");
    if(!f)
    {
        fprintf(stderr, "\nError: Cannot open %s.\n", path.c_str());
        return std::shared_ptr<Matrix>(nullptr);
    }
    std::shared_ptr<Matrix> M = read_matrix_meta(f);
    fclose(f);
    return M;
}

std::shared_ptr<Matrix> read_matrix(std::string const &path)
{
    FILE *f = fopen(path.c_str(), "rb");
    if(!f)
    {
        fprintf(stderr, "\nError: Cannot open %s.\n", path.c_str());
        return std::shared_ptr<Matrix>(nullptr);
    }
    std::shared_ptr<Matrix> M = read_matrix_meta(f);
    M->R.resize(M->nr_ratings);
    fread(M->R.data(), sizeof(Node), M->nr_ratings, f);
    fclose(f);
    return M;
}

bool write_matrix(Matrix const &M, std::string const &path)
{
    FILE *f = fopen(path.c_str(), "wb");
    if(!f)
    {
        fprintf(stderr, "\nError: Cannot open %s.\n", path.c_str());
        return false;
    }
    fwrite(&M.nr_users, sizeof(int), 1, f);
    fwrite(&M.nr_items, sizeof(int), 1, f);
    fwrite(&M.nr_ratings, sizeof(long), 1, f);
    fwrite(&M.avg, sizeof(float), 1, f);
    fwrite(M.R.data(), sizeof(Node), M.nr_ratings, f);
    fclose(f);
    return true;
}

Model::~Model()
{
    if(P != nullptr)
        free(P);
    if(Q != nullptr)
        free(Q);
}

std::shared_ptr<Model> read_model_meta(FILE *f)
{
    std::shared_ptr<Model> model(new Model);
    fread(&model->param, sizeof(Parameter), 1, f);
    fread(&model->nr_users, sizeof(int), 1, f);
    fread(&model->nr_items, sizeof(int), 1, f);
    fread(&model->avg, sizeof(float), 1, f);
    return model;
}

std::shared_ptr<Model> read_model_meta(std::string const &path)
{
    FILE *f = fopen(path.c_str(), "rb");
    if(!f)
    {
        fprintf(stderr, "\nError: Cannot open %s.\n", path.c_str());
        return std::shared_ptr<Model>(nullptr);
    }
    std::shared_ptr<Model> model = read_model_meta(f);
    fclose(f);
    return model;
}

std::shared_ptr<Model> read_model(std::string const &path)
{
    FILE *f = fopen(path.c_str(), "rb");
    if(!f)
    {
        fprintf(stderr, "\nError: Cannot open %s.\n", path.c_str());
        return std::shared_ptr<Model>(nullptr);
    }

    std::shared_ptr<Model> model = read_model_meta(f);
    int const dim_aligned = get_aligned_dim(model->param.dim);

    posix_memalign((void**)&model->P, 32,
                   model->nr_users*dim_aligned*sizeof(float));
    fread(model->P, sizeof(float), model->nr_users*dim_aligned, f);

    posix_memalign((void**)&model->Q, 32,
                   model->nr_items*dim_aligned*sizeof(float));
    fread(model->Q, sizeof(float), model->nr_items*dim_aligned, f);

    if(model->param.lub >= 0)
    {
        model->UB.resize(model->nr_users);
        fread(model->UB.data(), sizeof(float), model->nr_users, f);
    }

    if(model->param.lib >= 0) {
        model->IB.resize(model->nr_items);
        fread(model->IB.data(), sizeof(float), model->nr_items, f);
    }

    fclose(f);
    return model;
}

bool write_model(Model const &model, std::string const &path)
{
    FILE *f = fopen(path.c_str(), "wb");
    if(!f)
    {
        fprintf(stderr, "\nError: Cannot open %s.", path.c_str());
        return false;
    }
    int const dim_aligned = get_aligned_dim(model.param.dim);
    fwrite(&model.param, sizeof(Parameter), 1, f);
    fwrite(&model.nr_users, sizeof(int), 1, f);
    fwrite(&model.nr_items, sizeof(int), 1, f);
    fwrite(&model.avg, sizeof(float), 1, f);
    fwrite(model.P, sizeof(float), model.nr_users*dim_aligned, f);
    fwrite(model.Q, sizeof(float), model.nr_items*dim_aligned, f);
    if(model.param.lub >= 0)
        fwrite(model.UB.data(), sizeof(float), model.nr_users, f);
    if(model.param.lib >= 0)
        fwrite(model.IB.data(), sizeof(float), model.nr_items, f);
    fclose(f);
    return true;
}

float calc_rate(Model const &model, Node const &r)
{
    int const dim_aligned = get_aligned_dim(model.param.dim);
    float rate = std::inner_product(
                     model.P+r.uid*dim_aligned,
                     model.P+r.uid*dim_aligned + model.param.dim,
                     model.Q+r.iid*dim_aligned,
                     0.0);
    rate += model.avg;
    if(model.param.lub >= 0)
        rate += model.UB[r.uid];
    if(model.param.lib >= 0)
        rate += model.IB[r.iid];
    return rate;
}

float calc_rmse(const Model& model, Matrix const &M)
{
    double loss = 0;
    for(auto r = M.R.begin(); r != M.R.end(); r++)
    {
        float const e = r->rate - calc_rate(model, *r);
        loss += e*e;
    }
    return sqrt(loss/M.nr_ratings);
}

int get_aligned_dim(int const dim)
{
  return dim;
}

struct GriddedMatrix
{
    int nr_users, nr_items, nr_user_blocks, nr_item_blocks;
    long nr_ratings;
    float avg;
    std::vector<Matrix> GM;
};

std::vector<int> gen_map(int const size, bool const shuffle)
{
    std::vector<int> map(size, 0);
    for(int i = 0; i < size; i++)
        map[i] = i;
    if(shuffle)
        std::random_shuffle(map.begin(), map.end());
    return map;
}

Model generate_initial_model(Parameter const &param, int const nr_users,
                             int const nr_items, float const avg)
{
    int const dim_aligned = get_aligned_dim(param.dim);

    Model model;
    model.param = param;
    model.nr_users = nr_users;
    model.nr_items = nr_items;
    model.avg = avg;
    posix_memalign((void**)&model.P, 32,
                   model.nr_users*dim_aligned*sizeof(float));
    posix_memalign((void**)&model.Q, 32,
                   model.nr_items*dim_aligned*sizeof(float));

    auto initialize = [&] (float *ptr, int const count)
    {
        srand48(0L);
        for(int i = 0; i < count; i++)
        {
            int d = 0;
            for(; d < param.dim; d++, ptr++)
                *ptr = 0.1*drand48();
            for(; d < dim_aligned; d++, ptr++)
                *ptr = 0;
        }
    };

    initialize(model.P, model.nr_users);
    initialize(model.Q, model.nr_items);
    if(param.lub >= 0)
        model.UB.assign(nr_users, 0);
    if(param.lib >= 0)
        model.IB.assign(nr_items, 0);

    return model;
}

class Monitor
{
public:
    Monitor(GriddedMatrix const &Tr, Matrix const * const Va,
            Model const * const model, bool const show_tr_rmse,
            bool const show_obj);
    void scan_tr(GriddedMatrix const &Tr);
    void print_header();
    void print(int const iter, float const time, double const loss,
               float const tr_rmse);
    double calc_reg();
private:
    Matrix const * const Va;
    Model const *model;
    bool show_tr_rmse, show_obj;
    std::vector<int> nr_ratings_per_user, nr_ratings_per_item;
};

Monitor::Monitor(GriddedMatrix const &Tr, Matrix const * const Va,
                 Model const * const model, bool const show_tr_rmse,
                 bool const show_obj)
        : Va(Va), model(model), show_tr_rmse(show_tr_rmse),
          show_obj(show_obj), nr_ratings_per_user(0), nr_ratings_per_item(0)
{
    if(!show_obj)
        return;
    nr_ratings_per_user.assign(Tr.nr_users, 0);
    nr_ratings_per_item.assign(Tr.nr_items, 0);
    for(int u = 0; u < Tr.nr_user_blocks; u++)
        for(int i = 0; i < Tr.nr_item_blocks; i++)
        {
            Matrix const * const M = &Tr.GM[u*Tr.nr_item_blocks+i];
            for(long r = 0; r < M->nr_ratings; r++)
            {
                nr_ratings_per_user[M->R[r].uid]++;
                nr_ratings_per_item[M->R[r].iid]++;
            }
        }
}

void Monitor::print_header()
{
    char output[1024];
    sprintf(output, "%4s", "iter");
    sprintf(output+strlen(output), " %10s", "time");
    if(show_tr_rmse)
        sprintf(output+strlen(output), " %10s", "tr_rmse");
    if(Va != nullptr)
        sprintf(output+strlen(output), " %10s", "va_rmse");
    if(show_obj)
        sprintf(output+strlen(output), " %13s %13s %13s", "loss", "reg", "obj");
    printf("%s\n", output);
}

void Monitor::print(int const iter, float const time, double const loss,
                    float const tr_rmse)
{
    char output[1024];
    sprintf(output, "%-4d %10.2f", iter, time);
    if(show_tr_rmse)
        sprintf(output+strlen(output), " %10.3f", tr_rmse);
    if(Va != nullptr)
        sprintf(output+strlen(output), " %10.3f", calc_rmse(*model, *Va));
    if(show_obj)
    {
        double const reg = calc_reg();
        sprintf(output+strlen(output), " %13.3e %13.3e %13.3e", loss, reg,
                loss+reg);
    }
    printf("%s\n", output);
    fflush(stdout);
}

double Monitor::calc_reg()
{
    int const dim_aligned = get_aligned_dim(model->param.dim);
    double reg = 0;

    {
        float * const P = model->P;
        double reg_p = 0;
        for(int u = 0; u < model->nr_users; u++)
        {
            float * const p = P+u*dim_aligned;
            reg_p += nr_ratings_per_user[u] *
                     std::inner_product(p, p+model->param.dim, p, 0.0);
        }
        reg += reg_p*model->param.lp;
    }

    {
        float * const Q = model->Q;
        double reg_q = 0;
        for(int i = 0; i < model->nr_items; i++)
        {
            float * const q = Q+i*dim_aligned;
            reg_q += nr_ratings_per_item[i] *
                     std::inner_product(q, q+model->param.dim, q, 0.0);
        }
        reg += reg_q*model->param.lq;
    }

    if(model->param.lub >= 0)
    {
        double reg_ub = 0;
        for(int u = 0; u < model->nr_users; u++)
            reg_ub += nr_ratings_per_user[u] * model->UB[u] * model->UB[u];
        reg += reg_ub*model->param.lub;
    }

    if(model->param.lib >= 0)
    {
        double reg_ib = 0;
        for(int i = 0; i < model->nr_items; i++)
            reg_ib += nr_ratings_per_item[i] * model->UB[i] * model->UB[i];
        reg += reg_ib*model->param.lib;
    }

    return reg;
}

std::shared_ptr<GriddedMatrix> read_gridded_matrix(
        TrainOption const &option,
        std::vector<int> const &user_map,
        std::vector<int> const &item_map)
{
    FILE *f = fopen(option.tr_path.c_str(), "rb");
    if (!f)
    {
        fprintf(stderr, "\nError: Cannot open %s.\n", option.tr_path.c_str());
        return std::shared_ptr<GriddedMatrix>(nullptr);
    }
    std::shared_ptr<Matrix> Tr_meta = read_matrix_meta(f);

    std::shared_ptr<GriddedMatrix> Tr(new GriddedMatrix);
    Tr->nr_users = Tr_meta->nr_users;
    Tr->nr_items = Tr_meta->nr_items;
    Tr->nr_ratings = Tr_meta->nr_ratings;
    Tr->avg = Tr_meta->avg;
    Tr->nr_user_blocks = option.nr_user_blocks;
    Tr->nr_item_blocks = option.nr_item_blocks;
    Tr->GM.resize(option.nr_user_blocks*option.nr_item_blocks);

    std::vector<std::vector<Node>> buffers(Tr->nr_users);
    for(long r = 0; r < Tr->nr_ratings; r++)
    {
        Node node_in;
        fread(&node_in, sizeof(Node), 1, f);
        node_in.uid = user_map[node_in.uid];
        node_in.iid = item_map[node_in.iid];
        buffers[node_in.uid].push_back(node_in);
    }

    // It seems that sorting makes no significant difference in performance.
    /*
    std::mutex mtx;
    std::queue<int> tasks;
    for(int u = 0; u < Tr->nr_users; u++)
        tasks.push(u);

    auto sort_node = [] (Node lhs, Node rhs)
    {
        if(lhs.uid!=rhs.uid)
            return lhs.uid < rhs.uid;
        else
            return lhs.iid < rhs.iid;
    };

    auto sort_worker = [&] ()
    {
        while(true)
        {
            int u = 0;
            {
                std::lock_guard<std::mutex> lock(mtx);
                if(tasks.empty())
                    break;
                u = tasks.front();
                tasks.pop();
            }
            std::sort(buffers[u].begin(), buffers[u].end(), sort_node);
        }
    };

    std::vector<std::thread> threads;
    for(int tx = 0; tx < option.nr_threads; tx++)
        threads.push_back(std::thread(sort_worker));
    for(auto &thread : threads)
        thread.join();
    */

    int const seg_u = (int)ceil((double)Tr_meta->nr_users /
                                option.nr_user_blocks);
    int const seg_i = (int)ceil((double)Tr_meta->nr_items /
                                option.nr_item_blocks);
    for(auto &buffer : buffers)
    {
        for(auto &node : buffer)
        {
            int const bid = (node.uid/seg_u) * option.nr_item_blocks +
                            node.iid/seg_i;
            Tr->GM[bid].R.push_back(node);
        }
        buffer.clear();
    }

    for(auto &M : Tr->GM)
    {
        M.nr_ratings = (long)(M.R.size());
        M.R.shrink_to_fit();
    }

    return Tr;
}

class Scheduler
{
public:
    Scheduler(int const nr_user_blocks, int const nr_item_blocks,
              int const nr_threads);
    int get_job();
    void put_job(int const jid, double const loss);
    double get_loss();
    void wait_for_jobs_done(int const nr_jobs);
    void pause();
    void resume();
    void terminate();
    bool is_terminated();
private:
    void pause_if_needed();

    int const nr_user_blocks, nr_item_blocks, nr_blocks, nr_threads;
    int total_jobs, nr_paused_thrs;
    bool paused, terminated;
    std::vector<int> counts, order_u, order_i, blocked_u, blocked_i;
    std::vector<double> losses;
    std::mutex mtx;
    std::condition_variable cond_var;
};

Scheduler::Scheduler(int const nr_user_blocks, int const nr_item_blocks,
                     int const nr_threads)
        : nr_user_blocks(nr_user_blocks), nr_item_blocks(nr_item_blocks),
          nr_blocks(nr_user_blocks*nr_item_blocks), nr_threads(nr_threads),
          total_jobs(0), nr_paused_thrs(0), paused(false), terminated(false),
          counts(nr_blocks, 0), order_u(nr_user_blocks, 0),
          order_i(nr_item_blocks, 0), blocked_u(nr_user_blocks, 0),
          blocked_i(nr_item_blocks, 0), losses(nr_blocks, 0)
{
    for(int u = 0; u < nr_user_blocks; u++)
        order_u[u] = u;
    for(int i = 0; i < nr_item_blocks; i++)
        order_i[i] = i;
}

int Scheduler::get_job()
{
    int min_count = std::numeric_limits<int>::max();
    std::vector<int> candidates;
    candidates.reserve(nr_blocks);
    std::lock_guard<std::mutex> lock(mtx);
    for(int u = 0; u < nr_user_blocks; u++)
    {
        if(blocked_u[u] == 1)
            continue;
        for(int i = 0; i < nr_item_blocks; i++)
        {
            if(blocked_i[i] == 1)
                continue;
            int const jid = u*nr_item_blocks+i;
            int const count = counts[jid];
            if(count == min_count)
            {
                candidates.push_back(jid);
            }
            else if(count < min_count)
            {
                candidates.assign(1, jid);
                min_count = count;
            }
        }
    }
    int const best_jid = candidates[rand()%(int)candidates.size()];
    blocked_u[best_jid/nr_item_blocks] = 1;
    blocked_i[best_jid%nr_item_blocks] = 1;
    counts[best_jid]++;
    return best_jid;
}

void Scheduler::put_job(int const jid, double const loss)
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        blocked_u[jid/nr_item_blocks] = 0;
        blocked_i[jid%nr_item_blocks] = 0;
        losses[jid] = loss;
        total_jobs++;
        cond_var.notify_all();
    }
    pause_if_needed();
}

double Scheduler::get_loss()
{
    std::lock_guard<std::mutex> lock(mtx);
    return std::accumulate(losses.begin(), losses.end(), 0.0);
}

void Scheduler::wait_for_jobs_done(int const nr_jobs)
{
    std::unique_lock<std::mutex> lock(mtx);
    cond_var.wait(lock, [&]{return total_jobs >= nr_jobs;});
}

void Scheduler::pause()
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        paused = true;
    }
    std::unique_lock<std::mutex> lock(mtx);
    cond_var.wait(lock, [&]{return nr_paused_thrs == nr_threads;});
}

void Scheduler::pause_if_needed()
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        if(!paused)
            return;
        nr_paused_thrs++;
        cond_var.notify_all();
    }

    {
        std::unique_lock<std::mutex> lock(mtx);
        cond_var.wait(lock, [&]{return !paused;});
    }

    {
        std::lock_guard<std::mutex> lock(mtx);
        nr_paused_thrs--;
    }
}

void Scheduler::resume()
{
    {
        std::lock_guard<std::mutex> lock(mtx);
        paused = false;
    }
    cond_var.notify_all();
}

void Scheduler::terminate()
{
    std::lock_guard<std::mutex> lock(mtx);
    terminated = true;
}

bool Scheduler::is_terminated()
{
    std::lock_guard<std::mutex> lock(mtx);
    return terminated;
}

void sgd(GriddedMatrix const * const Tr, Model * const model,
         Scheduler * const scheduler)
{
    int const dim_aligned = get_aligned_dim(model->param.dim);
    bool const en_ub = (model->param.lub >= 0);
    bool const en_ib = (model->param.lib >= 0);
    float const glp = 1 - model->param.gamma*(model->param.lp);
    float const glq = 1 - model->param.gamma*(model->param.lq);
    float const glub = 1 - model->param.gamma*(model->param.lub);
    float const glib = 1 - model->param.gamma*(model->param.lib);
    float const gamma = model->param.gamma;
    float const avg = model->avg;

    float *const P = model->P;
    float *const Q = model->Q;
    float *const UB = model->UB.data();
    float *const IB = model->IB.data();

    int const dim = model->param.dim;
    while(true)
    {
        int const jid = scheduler->get_job();
        double loss = 0;
        Matrix const * const M = &Tr->GM[jid];
        float *p, *q, *ub, *ib;
        for(auto r = M->R.begin(); r != M->R.end(); r++)
        {
            p = P + r->uid*dim_aligned;
            q = Q + r->iid*dim_aligned;
            float ge = std::inner_product(p, p+dim_aligned, q, 0.0) + avg;
            if (en_ub)
            {
                ub = UB + r->uid;
                ge += (*ub);
            }
            if (en_ib)
            {
                ib = IB + r->iid;
                ge += (*ib);
            }
            ge = r->rate - ge;
            loss += ge*ge;
            ge *= gamma;
            for(int d = 0; d < dim; d++)
            {
                float const tmp = p[d];
                p[d] = ge*q[d] + glp*p[d];
                q[d] = ge*tmp + glq*q[d];
            }
            if(en_ub)
                *ub = glub*(*ub) + ge;
            if(en_ib)
                *ib = glib*(*ib) + ge;
        }
        scheduler->put_job(jid, loss);
        if(scheduler->is_terminated())
            break;
    }
}

Model fpsgd(GriddedMatrix const &Tr, Matrix const &Va,
            TrainOption const &option)
{
    Timer timer;
    timer.reset("Initializing model...");
    Model model = generate_initial_model(option.param, Tr.nr_users, Tr.nr_items,
                                         option.use_avg? Tr.avg : 0);
    timer.toc("done.");

    Monitor monitor(Tr, &Va, &model, option.show_tr_rmse,
                    option.show_obj);

    Scheduler scheduler(option.nr_user_blocks, option.nr_item_blocks,
                        option.nr_threads);
    std::vector<std::thread> threads;
    for(int tx = 0; tx < option.nr_threads; tx++)
        threads.push_back(std::thread(sgd, &Tr, &model, &scheduler));
    monitor.print_header();

    timer.reset();
    for(int iter = 1; iter <= option.nr_iters; iter++)
    {
        scheduler.wait_for_jobs_done(iter * option.nr_user_blocks *
                                     option.nr_item_blocks);
        scheduler.pause();
        float const iter_time = timer.toc();
        double const loss = scheduler.get_loss();
        monitor.print(iter, iter_time, loss, sqrt(loss/Tr.nr_ratings));
        timer.tic();
        scheduler.resume();
    }

    scheduler.terminate();
    for(auto thread = threads.begin(); thread != threads.end(); thread++)
        thread->join();

    return model;
}

void inversely_shuffle_model(Model &model, std::vector<int> const &user_map,
                             std::vector<int> const &item_map)
{
    auto gen_inv_map = [] (std::vector<int> const &map)
    {
        std::vector<int> inv_map(map.size());
        for(int i = 0; i < (int)map.size(); i++)
          inv_map[map[i]] = i;
        return inv_map;
    };

    auto shuffle = [] (float * const vec, std::vector<int> const &map,
                       int const count, int const dim)
    {
        std::vector<float> vec_(count*dim);
        std::copy(vec, vec+count*dim, vec_.data());
        for(int idx = 0; idx < count; idx++)
            std::copy(vec_.data()+idx*dim,
                      vec_.data()+idx*dim+dim,
                      vec+map[idx]*dim);
    };

    std::vector<int> const inv_user_map = gen_inv_map(user_map);
    std::vector<int> const inv_item_map = gen_inv_map(item_map);

    int const dim_aligned = get_aligned_dim(model.param.dim);

    shuffle(model.P, inv_user_map, model.nr_users, dim_aligned);
    shuffle(model.Q, inv_item_map, model.nr_items, dim_aligned);
    if(model.param.lub >= 0)
        shuffle(model.UB.data(), inv_user_map, model.nr_users, 1);
    if(model.param.lib >= 0)
        shuffle(model.IB.data(), inv_item_map, model.nr_items, 1);
}

void SVDTrain::Train() {
    LoadConfig();
    std::shared_ptr<const TrainOption> const option = ParseTrainOption();
     Timer timer;

     std::shared_ptr<const Matrix> const
         Tr_meta = read_matrix_meta(option->tr_path);
     if(!Tr_meta)
         return ;

     std::vector<int> const user_map = gen_map(Tr_meta->nr_users,
                                               option->rand_shuffle);
     std::vector<int> const item_map = gen_map(Tr_meta->nr_items,
                                               option->rand_shuffle);

     timer.reset("Reading training data...");
     std::shared_ptr<const GriddedMatrix> const
         Tr = read_gridded_matrix(*option, user_map, item_map);
     if(!Tr)
         return ;
     timer.toc("done.");

     std::shared_ptr<Matrix> Va;
     if(!option->va_path.empty())
     {
         timer.reset("Reading validation data...");
         Va = read_matrix(option->va_path);
         if(!Va)
             return ;
         timer.toc("done.");
         if(Va->nr_users > Tr_meta->nr_users || Va->nr_items > Tr_meta->nr_items)
         {
             fprintf(stderr, "Error: Validation set out of range.\n");
             return ;
         }
         for (auto &r : Va->R)
         {
             r.uid = user_map[r.uid];
             r.iid = item_map[r.iid];
         }
     }

     Model model = fpsgd(*Tr, *Va, *option);

     if(option->rand_shuffle)
         inversely_shuffle_model(model, user_map, item_map);

     timer.reset("Writing model...");
     if(!write_model(model, option->model_path))
         return ;
     timer.toc("done.");
}

std::shared_ptr<TrainOption> SVDTrain::ParseTrainOption() {
    std::shared_ptr<TrainOption> option(new TrainOption);
    option->nr_user_blocks = 0;
    option->nr_item_blocks = 0;

    option->param.dim = mConfig["dimensions"].asInt();
    assert(option->param.dim > 0);

    option->nr_iters = mConfig["iterations"].asInt();
    assert(option->nr_iters > 0);

    option->nr_threads = mConfig["threads"].asInt();
    assert(option->nr_threads > 0);

    option->param.lp = mConfig["lambdaP"].asDouble();
    assert(option->param.lp >= 0.0);

    option->param.lq = mConfig["lambdaQ"].asDouble();
    assert(option->param.lq >= 0.0);

    option->param.gamma = mConfig["gamma"].asDouble();
    assert(option->param.gamma > 0.0);

    std::string blk_str(mConfig["blocks"].asString());
    const char *p = strtok(&*blk_str.begin(), "x");
    option->nr_user_blocks = atoi(p);
    p = strtok(nullptr, "x");
    option->nr_item_blocks = atoi(p);
    assert(option->nr_user_blocks > 0 && option->nr_item_blocks > 0);

    option->rand_shuffle = mConfig["randShuffle"].asBool();

    option->show_tr_rmse = mConfig["trainRMSE"].asBool();

    option->show_obj = mConfig["objValue"].asBool();

    option->use_avg = mConfig["useAvg"].asBool();

    option->param.lub = mConfig["lambdaUb"].asDouble();

    option->param.lib = mConfig["lambdaIb"].asDouble();

    if(option->nr_user_blocks == 0) {
        option->nr_user_blocks = 2*option->nr_threads;
    }

    if(option->nr_item_blocks == 0) {
        option->nr_item_blocks = 2*option->nr_threads;
    }

    assert(option->nr_user_blocks > option->nr_threads);
    assert(option->nr_item_blocks > option->nr_threads);

    option->tr_path = mRatingTrainFilepath;
    option->model_path = mModelFilepath;
    return option;
}

} //~ namespace longan
