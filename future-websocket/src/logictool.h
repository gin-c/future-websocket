#ifndef __LOGIC_TOOL_H__
#define __LOGIC_TOOL_H__
#include <string>
#include <vector>

namespace logictool {

class CLogicTool
{
public:
	CLogicTool();
	~CLogicTool();

	std::string base64Encode(const char * bytes, uint32_t length);
	std::string base64Decode(const std::string& encoded);

	std::string image2base64(const std::string &path);
	bool base64toimage(const std::string &base64, const std::string &dstpath);

	template<typename T, typename... Args>
	std::string generateTopic(const std::string& topic, const T& head, Args... args) {
		std::string tmpTopic(topic);
		size_t pos = tmpTopic.find('+');
		if (pos == -1) {
			return topic;
		}
		tmpTopic.replace(pos, 1, head);
		return generateTopic(tmpTopic, args...);
	}
	std::string	 generateTopic(const std::string& topic) { return topic; }
	std::string now();
	uint64_t nowMilliseconds();
	std::string millisecond2StrTime(uint64_t u64Milliseconds);
	uint64_t steadyClock();
	void strTime2numTime(const std::string& strTime, OUT uint32_t& u32Year, OUT uint32_t& u32Mon, OUT uint32_t& u32Day, OUT uint32_t& u32Hour, OUT uint32_t& u32Minute, OUT uint32_t& u32Second);
	std::string numTime2StrTime(uint32_t u32Year, uint32_t u32Mon, uint32_t u32Day, uint32_t u32Hour, uint32_t u32Minute, uint32_t u32Second);
	std::string generateUUID();
	std::vector<std::string> splitString(std::string srcStr, std::string delimStr, bool repeatedCharIgnored);
	uint16_t crc16(uint16_t crc, const char* buf, int len);
	uint64_t crc64(uint64_t crc, const unsigned char* s, uint64_t l);
private:
	inline BOOL isBase64(const char c);
};

}

#endif //__LOGIC_TOOL_H__
