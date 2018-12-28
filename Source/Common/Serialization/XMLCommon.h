#ifndef XMLCOMMON_H
#define XMLCOMMON_H

#include <tinyxml2.h>

inline const char* XMLErrorString(tinyxml2::XMLError Error)
{
    switch (Error)
    {
    case tinyxml2::XML_SUCCESS:                         return "XML_SUCCESS";
    case tinyxml2::XML_NO_ATTRIBUTE:                    return "XML_NO_ATTRIBUTE";
    case tinyxml2::XML_ERROR_FILE_NOT_FOUND:            return "XML_ERROR_FILE_NOT_FOUND";
    case tinyxml2::XML_ERROR_FILE_COULD_NOT_BE_OPENED:  return "XML_ERROR_FILE_COULD_NOT_BE_OPENED";
    case tinyxml2::XML_ERROR_FILE_READ_ERROR:           return "XML_ERROR_FILE_READ_ERROR";
    case tinyxml2::XML_ERROR_PARSING_ELEMENT:           return "XML_ERROR_PARSING_ELEMENT";
    case tinyxml2::XML_ERROR_PARSING_ATTRIBUTE:         return "XML_ERROR_PARSING_ATTRIBUTE";
    case tinyxml2::XML_ERROR_PARSING_TEXT:              return "XML_ERROR_PARSING_TEXT";
    case tinyxml2::XML_ERROR_PARSING_CDATA:             return "XML_ERROR_PARSING_CDATA";
    case tinyxml2::XML_ERROR_PARSING_COMMENT:           return "XML_ERROR_PARSING_COMMENT";
    case tinyxml2::XML_ERROR_PARSING_DECLARATION:       return "XML_ERROR_PARSING_DECLARATION";
    case tinyxml2::XML_ERROR_PARSING_UNKNOWN:           return "XML_ERROR_PARSING_UNKNOWN";
    case tinyxml2::XML_ERROR_EMPTY_DOCUMENT:            return "XML_ERROR_EMPTY_DOCUMENT";
    case tinyxml2::XML_ERROR_MISMATCHED_ELEMENT:        return "XML_ERROR_MISMATCHED_ELEMENT";
    case tinyxml2::XML_ERROR_PARSING:                   return "XML_ERROR_PARSING";
    case tinyxml2::XML_CAN_NOT_CONVERT_TEXT:            return "XML_CAN_NOT_CONVERT_TEXT";
    case tinyxml2::XML_NO_TEXT_NODE:                    return "XML_NO_TEXT_NODE";
    case tinyxml2::XML_ELEMENT_DEPTH_EXCEEDED:          return "XML_ELEMENT_DEPTH_EXCEEDED";
    default:                                            return "<Unknown tinyxml2::XMLError>";
    }
}

#endif // XMLCOMMON_H
