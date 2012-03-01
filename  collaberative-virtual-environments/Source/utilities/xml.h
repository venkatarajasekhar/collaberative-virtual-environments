/**
 * The XMLFile class deals with xml files
 */

#ifndef XML_H
#define XML_H

#include <map>
#include <string>
#include <tinyxml.h>

class XMLFile
{
public:
    XMLFile();
	~XMLFile();

    bool load(const std::string &file);
	void close();
    bool nextElement(const std::string &element);
    bool nextSubElement(const std::string &element);
    void setElement(const std::string &element);
    void setSubElement(const std::string &subelement);
    std::string readString(const std::string &element, const std::string &attribute);
    int readInt(const std::string &element, const std::string &attribute);
	void changeString(const std::string &element, const std::string &attribute,
						const std::string &value);
	void changeInt(const std::string &element, const std::string &attribute, int value);
	void addString(const std::string &element, const std::string &attribute,
						const std::string &value);
    void addInt(const std::string &element, const std::string &attribute, int value);
	void clear(const std::string &element);
	bool parse(char *data);
	void save();

private:
    TiXmlDocument *mDoc;
    TiXmlHandle *mHandle;
    std::map<std::string, TiXmlElement*> mElements;
    TiXmlElement *mCurrentElement;
    typedef std::map<std::string, TiXmlElement*>::iterator ElementItr;
};

#endif
