#ifndef UFSTREAM_H
#define UFSTREAM_H

#include <stdio.h>
#ifdef __MINGW32__

#include <ext/stdio_filebuf.h>
#include <share.h>

class char_filebuf : public __gnu_cxx::stdio_filebuf<char, std::char_traits<char>> {
    public:
    char_filebuf() : stdio_filebuf() {}
    char_filebuf(FILE* file, std::ios::openmode mode) : stdio_filebuf(file, mode) {}
    virtual ~char_filebuf() {
        fclose(this->_M_file.file());
    }
};

class uifstream : public std::basic_istream<char, std::char_traits<char>> {
    public:
    uifstream() : basic_istream(NULL) {
        setstate(ios_base::badbit);
    }
    uifstream(const boost::filesystem::path& path, std::ios::openmode mode = std::ios::in) : basic_istream(NULL) {
        open(path, mode);
    }

    void open(const boost::filesystem::path& path, std::ios::openmode mode = std::ios::in) {
        std::wstring modestr = L"r";
        mode |= std::ios::in;
        if (mode & std::ios::binary)
            modestr += L"b";
        FILE* file = _wfsopen(path.wstring().c_str(), modestr.c_str(), _SH_DENYNO);
        if (file == NULL) {
            setstate(ios_base::failbit);
        } else {
            rdbuf(new char_filebuf(file, mode));
            clear();
        }
    }
    virtual ~uifstream() {
        delete rdbuf();
    }
};

class uofstream : public std::basic_ostream<char, std::char_traits<char>> {
    public:
    uofstream() : basic_ostream(NULL) {
        setstate(ios_base::badbit);
    }
    uofstream(const boost::filesystem::path& path, std::ios::openmode mode = std::ios::out) : basic_ostream(NULL) {
        open(path, mode);
    }

    void open(const boost::filesystem::path& path, std::ios::openmode mode = std::ios::out) {
        std::wstring modestr = L"w";
        mode |= std::ios::out;
        if (mode & std::ios::app) {
            modestr = L"a";
        }
        if (mode & std::ios::binary)
            modestr += L"b";
        FILE* file = _wfsopen(path.wstring().c_str(), modestr.c_str(), _SH_DENYNO);
        if (file == NULL) {
            setstate(ios_base::badbit);
        } else {
            rdbuf(new char_filebuf(file, mode));
            clear();
        }
    }

    virtual ~uofstream() {
        delete rdbuf();
    }
};

#else // __MINGW32__

typedef boost::filesystem::ifstream uifstream;
typedef boost::filesystem::ofstream uofstream;

#endif // __MINGW32__

#endif // UFSTREAM_H
