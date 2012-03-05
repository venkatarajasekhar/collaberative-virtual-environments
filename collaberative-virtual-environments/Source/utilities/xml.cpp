#include <iostream>

#include "xml.h"

XMLFile::XMLFile() : mDoc(NULL), mHandle(NULL), mCurrentElement(NULL)
{

}

XMLFile::~XMLFile()
{
	mElements.clear();
	close();
}

bool XMLFile::load(const std::string &file)
{
	mDoc = new TiXmlDocument(file.c_str());
    bool loaded = mDoc->LoadFile();
    if (!loaded)
    {
		std::cerr << "File not found: " << file << std::endl;
        return false;
    }

    mHandle = new TiXmlHandle(mDoc);
    return true;
}

void XMLFile::close()
{
	delete mHandle;
	mHandle = NULL;
	delete mDoc;
	mDoc = NULL;
}

bool XMLFile::nextSubElement(const std::string &element)
{
    ElementItr itr = mElements.find(element);
    if (itr != mElements.end())
    {
        itr->second = itr->second->NextSiblingElement();
	    return itr->second != 0;
    }
    return false;
}

bool XMLFile::nextElement(const std::string &element)
{
    ElementItr itr = mElements.find(element);
    if (itr != mElements.end())
    {
        itr->second = itr->second->NextSiblingElement();
        mCurrentElement = itr->second;
	    return itr->second != 0;
    }
    return false;
}

void XMLFile::setElement(const std::string &element)
{
	assert(mHandle);
	TiXmlElement *e = mHandle->FirstChild(element.c_str()).Element();
	if (e)
	{
	    mElements.insert(std::pair<std::string, TiXmlElement*>(element, e));
        mCurrentElement = e;
	}
}

void XMLFile::setSubElement(const std::string &subelement)
{
	assert(mCurrentElement);
    TiXmlElement *e = mCurrentElement->FirstChildElement(subelement.c_str());
    mElements.insert(std::pair<std::string, TiXmlElement*>(subelement, e));
}

std::string XMLFile::readString(const std::string &element, const std::string &attribute)
{
    std::string str;

    ElementItr itr = mElements.find(element);
    if (itr != mElements.end())
    {
        str = itr->second->Attribute(attribute.c_str());
    }

    return str;
}

int XMLFile::readInt(const std::string &element, const std::string &attribute)
{
    int value = 0;

    ElementItr itr = mElements.find(element);
    if (itr != mElements.end())
    {
        itr->second->QueryIntAttribute(attribute.c_str(), &value);
    }
    return value;
}

void XMLFile::changeString(const std::string &element, const std::string &attribute,
    const std::string &value)
{
    ElementItr itr = mElements.find(element);
    if (itr != mElements.end())
    {
        itr->second->SetAttribute(attribute.c_str(), value.c_str());
    }
}

void XMLFile::changeInt(const std::string &element, const std::string &attribute, int value)
{
    ElementItr itr = mElements.find(element);
    if (itr != mElements.end())
    {
        itr->second->SetAttribute(attribute.c_str(), value);
    }
}

void XMLFile::addString(const std::string &element, const std::string &attribute,
    const std::string &value)
{
    TiXmlElement *el = new TiXmlElement(element.c_str());
    el->SetAttribute(attribute.c_str(), value.c_str());
    mDoc->LinkEndChild(el);
}

void XMLFile::addInt(const std::string &element, const std::string &attribute, int value)
{
    TiXmlElement *el = new TiXmlElement(element.c_str());
    el->SetAttribute(attribute.c_str(), value);
    mDoc->LinkEndChild(el);
}

void XMLFile::clear(const std::string &element)
{
    ElementItr itr = mElements.find(element);
    if (itr != mElements.end())
    {
        mElements.erase(itr);
    }
}

bool XMLFile::parse(char *data)
{
    mDoc = new TiXmlDocument;
    mDoc->Parse(data);
    mHandle = new TiXmlHandle(mDoc);
    return true;
}

void XMLFile::save()
{
    mDoc->SaveFile();
}