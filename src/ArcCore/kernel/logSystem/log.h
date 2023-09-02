
#ifndef LOG_H
#define LOG_H




// ��־����ö��
enum LOGLEVEL 
{
	LOG_LEVEL_ALL = 0,
	LOG_LEVEL_TRACE,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARN,
	LOG_LEVEL_ERROR
};

// ��־���λ��
enum LOGTARGET 
{
	LOG_TARGET_NONE	   = 0x00,
	LOG_TARGET_CONSOLE = 0x01,
	LOG_TARGET_FILE	   = 0x10
};


class Log
{
public:
	virtual int LogInit() = 0;
};


// ������־��
#define LOG(level, message) LogManager::instance().log(level, message, __FILE__, __LINE__)


#define LEVEL_DEBUG						// ����
#define LEVEL_INFO 						// ��ʾ
#define LEVEL_WARNING					// ����
#define LEVEL_ERROR						// ����

#endif // !LOG_H