#ifndef U_FSTREAM_H
#define U_FSTREAM_H

#include <fstream>
#include <boost/filesystem/fstream.hpp>

// fs::fstream ctor doesn't support wide pathnames on mingw.
// So we roll our own. Based on std::fstream implementation in gcc.
#ifdef __MINGW32__
    #include <stdio.h> // _wfsopen()
    #include <ext/stdio_filebuf.h> // stdio_filebuf
    #include <share.h> // _SH_DENYNO

namespace std {
  template<typename _CharT, ios_base::openmode default_mode = ios_base::in | ios_base::out,
      typename _Traits = std::char_traits<_CharT>>
    class u_basic_fstream : public std::basic_iostream<_CharT, _Traits>
    {
    public:
      // Types:
      typedef _CharT                                    char_type;
      typedef _Traits                                   traits_type;
      typedef typename traits_type::int_type            int_type;
      typedef typename traits_type::pos_type            pos_type;
      typedef typename traits_type::off_type            off_type;

      // Non-standard types:
      typedef __gnu_cxx::stdio_filebuf<char_type, traits_type>     __filebuf_type;
      typedef basic_ios<char_type, traits_type>         __ios_type;
      typedef basic_iostream<char_type, traits_type>    __iostream_type;

    private:
      __filebuf_type    _M_filebuf;

    public:
      // Constructors/destructor:
      /**
       *  @brief  Default constructor.
       *
       *  Initializes @c sb using its default constructor, and passes
       *  @c &sb to the base class initializer.  Does not open any files
       *  (you haven't given it a filename to open).
       */
      u_basic_fstream()
      : __iostream_type(), _M_filebuf()
      { this->init(&_M_filebuf); }

      /**
       *  @brief  Create an input/output file stream.
       *  @param  __s  Null terminated string specifying the filename.
       *  @param  __mode  Open file in specified mode (see std::ios_base).
       *
       *  Tip:  When using std::string to hold the filename, you must use
       *  .c_str() before passing it to this constructor.
       */
      explicit
      u_basic_fstream(const wchar_t* __s,
                    ios_base::openmode __mode = ios_base::in | ios_base::out)
      : __iostream_type(0), _M_filebuf()
      {
        this->init(&_M_filebuf);
        this->open(__s, __mode);
      }

#if __cplusplus >= 201103L
      /**
       *  @brief  Create an input/output file stream.
       *  @param  __s  Null terminated string specifying the filename.
       *  @param  __mode  Open file in specified mode (see std::ios_base).
       */
      explicit
      u_basic_fstream(const std::wstring& __s,
                    ios_base::openmode __mode = ios_base::in | ios_base::out)
      : __iostream_type(0), _M_filebuf()
      {
        this->init(&_M_filebuf);
        this->open(__s, __mode);
      }
      explicit
      u_basic_fstream(const boost::filesystem::path& __s,
                    ios_base::openmode __mode = ios_base::in | ios_base::out)
      : __iostream_type(0), _M_filebuf()
      {
        this->init(&_M_filebuf);
        this->open(__s, __mode);
      }
#endif

      /**
       *  @brief  The destructor does nothing.
       *
       *  The file is closed by the filebuf object, not the formatting
       *  stream.
       */
      ~u_basic_fstream()
      { }

      // Members:
      /**
       *  @brief  Accessing the underlying buffer.
       *  @return  The current basic_filebuf buffer.
       *
       *  This hides both signatures of std::basic_ios::rdbuf().
       */
      __filebuf_type*
      rdbuf() const
      { return const_cast<__filebuf_type*>(&_M_filebuf); }

      /**
       *  @brief  Wrapper to test for an open file.
       *  @return  @c rdbuf()->is_open()
       */
      bool
      is_open()
      { return _M_filebuf.is_open(); }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 365. Lack of const-qualification in clause 27
      bool
      is_open() const
      { return _M_filebuf.is_open(); }

      /**
       *  @brief  Opens an external file.
       *  @param  __s  The name of the file.
       *  @param  __mode  The open mode flags.
       *
       *  Calls @c std::basic_filebuf::open(__s,__mode).  If that
       *  function fails, @c failbit is set in the stream's error state.
       *
       *  Tip:  When using std::string to hold the filename, you must use
       *  .c_str() before passing it to this constructor.
       */
      void
      open(const wchar_t* __s,
           ios_base::openmode __mode = default_mode)
      {
        // An incomplete mapping between iostreams mode and fopen() mode.
        std::wstring modestr;
        if (__mode & ios_base::in)
            modestr += L"r";
        if (__mode & ios_base::app)
            modestr += L"a";
        else if (__mode & ios_base::out)
            modestr += L"w";
        if (__mode & ios_base::binary)
            modestr += L"b";
        FILE* file = _wfsopen(__s, modestr.c_str(), _SH_DENYNO);
        int fd = fileno(file); // the file pointer is leaked
        __filebuf_type _M_filebuf(fd, __mode);
        if (!_M_filebuf.is_open())
          this->setstate(ios_base::failbit);
        else
          // _GLIBCXX_RESOLVE_LIB_DEFECTS
          // 409. Closing an fstream should clear error state
          this->clear();
      }

#if __cplusplus >= 201103L
      /**
       *  @brief  Opens an external file.
       *  @param  __s  The name of the file.
       *  @param  __mode  The open mode flags.
       *
       *  Calls @c std::basic_filebuf::open(__s,__mode).  If that
       *  function fails, @c failbit is set in the stream's error state.
       */
      void
      open(const std::wstring& __s,
           ios_base::openmode __mode = default_mode)
      {
          open(__s.c_str(), __mode);
      }
      void
      open(const boost::filesystem::path& __s,
           ios_base::openmode __mode = default_mode)
      {
          open(__s.wstring(), __mode);
      }
#endif

      /**
       *  @brief  Close the file.
       *
       *  Calls @c std::basic_filebuf::close().  If that function
       *  fails, @c failbit is set in the stream's error state.
       */
      void
      close()
      {
        if (!_M_filebuf.close())
          this->setstate(ios_base::failbit);
      }
    };
};

    typedef std::u_basic_fstream<char, std::ios::in> u_ifstream;
    typedef std::u_basic_fstream<char, std::ios::out> u_ofstream;
#else
    typedef boost::filesystem::ifstream u_ifstream;
    typedef boost::filesystem::ofstream u_ofstream;
#endif
#endif // U_FSTREAM_H
