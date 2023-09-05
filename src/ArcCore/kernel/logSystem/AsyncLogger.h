/*
* 
* 将日志写入到一个缓冲队列中，由另一个独立的线程专门负责将日志数据写入到文件系统中。
* 这种模式称为“前后端模式”，
* 
* 
* 优点是：将日志写入到文件系统这种耗时大的动作不会影响到业务程序的运行。
* 
* 
*/

// 异步日志

/*
+-------------------------------------+
|           Application               |
+-------------------------------------+
               |
               v
+-------------------------------------+
|         Logger (Async)              |
+-------------------------------------+
               |
               v
+-------------------------------------+
|        Log Buffer (Circular)        |
|       +-------------------+         |
|       | Log Message 1     |         |
|       | Log Message 2     |         |
|       | ...               |         |
|       | Log Message N     |         |
|       +-------------------+         |
+-------------------------------------+
               |
               v
+-------------------------------------+
|        Writer Thread                |
+-------------------------------------+
               |
               v
+-------------------------------------+
|        Log File (or Database)       |
|       +-------------------+         |
|       | Log Message 1     |         |
|       | Log Message 2     |         |
|       | ...               |         |
|       | Log Message N     |         |
|       +-------------------+         |
+-------------------------------------+

1. 应用程序产生日志消息并将其发送给日志记录器(Logger)。

2. 日志记录器将日志消息存储在日志缓冲(Buffer)中。

3. 日志写入器(Writer Thread)定期或根据一定条件将缓冲区中的日志消息写入到永久存储介质中（例如日志文件或数据库）。

4. 日志格式化器(Formatter)负责将日志消息格式化为可读的文本，并存储在永久存储介质中。

5. 配置器(Configurator)用于配置日志系统的参数，例如设置日志级别、指定日志输出路径等。

*/

#ifndef ASYNCLOGGER_H
#define ASYNCLOGGER_H

class AsyncLogger
{
public:
	AsyncLogger();
	~AsyncLogger();
};

#endif // !ASYNCLOGGER_H