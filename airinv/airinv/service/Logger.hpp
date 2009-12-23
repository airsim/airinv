#ifndef __AIRINV_SVC_LOGGER_HPP
#define __AIRINV_SVC_LOGGER_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <sstream>
#include <string>
// AIRINV
#include <airinv/AIRINV_Types.hpp>

// /////////////// LOG MACROS /////////////////
#define AIRINV_LOG_CORE(iLevel, iToBeLogged) \
  { std::ostringstream ostr; ostr << iToBeLogged; \
    AIRINV::Logger::instance().log (iLevel, __LINE__, __FILE__, ostr.str()); }

#define AIRINV_LOG_CRITICAL(iToBeLogged) \
  AIRINV_LOG_CORE (AIRINV::LOG::CRITICAL, iToBeLogged)

#define AIRINV_LOG_ERROR(iToBeLogged) \
  AIRINV_LOG_CORE (AIRINV::LOG::ERROR, iToBeLogged)

#define AIRINV_LOG_NOTIFICATION(iToBeLogged) \
  AIRINV_LOG_CORE (AIRINV::LOG::NOTIFICATION, iToBeLogged)

#define AIRINV_LOG_WARNING(iToBeLogged) \
  AIRINV_LOG_CORE (AIRINV::LOG::WARNING, iToBeLogged)

#define AIRINV_LOG_DEBUG(iToBeLogged) \
  AIRINV_LOG_CORE (AIRINV::LOG::DEBUG, iToBeLogged)

#define AIRINV_LOG_VERBOSE(iToBeLogged) \
  AIRINV_LOG_CORE (AIRINV::LOG::VERBOSE, iToBeLogged)
// /////////// (END OF) LOG MACROS /////////////


namespace AIRINV {

  /** Class holding the stream for logs. 
      <br>Note that the error logs are seen as standard output logs, 
      but with a higher level of visibility. */
  class Logger {
    // Friend classes
    friend class FacSupervisor;
  public:
    
    /** Main log entry. */
    template <typename T>
    void log (const LOG::EN_LogLevel iLevel, const int iLineNumber,
              const std::string& iFileName, const T& iToBeLogged) {
      if (iLevel <= _level) {
        assert (_logStream != NULL);
        *_logStream << iFileName << ":" << iLineNumber
                    << ": " << iToBeLogged << std::endl;
      }
    }
    
    /** Get the log level. */
    LOG::EN_LogLevel getLogLevel();
    
    /** get the log stream. */
    std::ostream& getLogStream();
    
    /** Set the logger parameters (level and stream). */
    void setLogParameters (const LOG::EN_LogLevel iLogLevel, 
                           std::ostream& ioLogStream);
    
    /** Returns a current Logger instance.*/
    static Logger& instance();
    
  private:
    /** Default constructors are private so that only the required 
        constructor can be used. */
    Logger ();
    Logger (const Logger&);
    Logger (const LOG::EN_LogLevel iLevel, std::ostream& ioLogStream);
    /** Destructor. */
    ~Logger ();
    
  private:
    /** Log level. */
    LOG::EN_LogLevel _level;
    
    /** Stream dedicated to the logs. */
    std::ostream* _logStream;
    
    /** Instance object.*/
    static Logger* _instance;
  };
  
}
#endif // __AIRINV_SVC_LOGGER_HPP
