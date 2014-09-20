#include "file_util.h"
#include "common/error.h"
#include <iostream>
#include <fstream>
#include <cstdio>

namespace longan {

namespace FileUtil {

void Copy(const std::string& srcFilename, const std::string& dstFilename) {
    FILE *srcfp = fopen(srcFilename.c_str(), "r");
    FILE *dstfp = fopen(dstFilename.c_str(), "w");
    if (srcfp == nullptr || dstfp == nullptr) {
        throw LonganFileOpenError();
    }
    int c;
    c = getc(srcfp);
    while (c != EOF) {
        putc(c, dstfp);
        c = getc(srcfp);
    }
    fclose(srcfp);
    fclose(dstfp);
}

void Delete(const std::string& filename) {
    if (remove(filename.c_str()) != 0) {
        throw LonganFileDeleteError();
    }
}

bool IsContentSame(const std::string& filename1, const std::string& filename2) {
    FILE* fp1 = fopen(filename1.c_str(), "r");
    FILE* fp2 = fopen(filename2.c_str(), "r");
    if (fp1 == nullptr || fp2 == nullptr) {
        throw LonganFileOpenError();
    }
    while (true) {
        int ch1 = fgetc(fp1);
        int ch2 = fgetc(fp2);
        if (ch1 != ch2) {
            fclose(fp1);
            fclose(fp2);
            return false;
        }
        if (ch1 == EOF) {
            fclose(fp1);
            fclose(fp2);
            return true;
        }
    }
    return true;
}

std::string LoadFileContent(const std::string& filename) {
    std::ifstream fin(filename.c_str());
    if (fin.fail()) {
    	throw LonganFileOpenError();
    }
    std::string content;
    std::string line;
    while (std::getline(fin, line)) {
        content.append(line).append("\n");
    }
    fin.close();
    return content;
}

void SaveFileContent(const std::string& filename, const std::string& content) {
    std::ofstream fout(filename.c_str());
    if (fout.fail()) {
    	throw LonganFileOpenError();
    }
    fout << content;
    fout.close();
}

int LineCount(const std::string& filename) {
    FILE *fp = fopen(filename.c_str(), "r");
    if (fp == nullptr) {
        throw LonganFileOpenError();
    }
    int c, numLine = 0;
	while ((c = getc(fp)) != EOF) {
		if (c == '\n') {
		   ++numLine;
		}
	}
	++numLine;
	return numLine;
}

int WordCount(const std::string& filename) {
    static const int OUT = 1;
	static const int IN = 0;
    FILE *fp = fopen(filename.c_str(), "r");
	int c;
	int state = OUT;
	int numWord = 0;
	while ((c = getc(fp)) != EOF) {
		if (c == ' ' || c == '\n' || c == '\t') {
			state = OUT;
		} else if (state == OUT) {
			state = IN;
			++numWord;
	   }
	}
	return numWord;
}

} //~ namespace FileUtil

} //~ namespace longan
