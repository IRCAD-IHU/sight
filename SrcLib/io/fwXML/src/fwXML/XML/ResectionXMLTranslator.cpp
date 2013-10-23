/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/lexical_cast.hpp>

#include <fwData/Resection.hpp>

#include "fwXML/XML/ResectionXMLTranslator.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/Serializer.hpp"

namespace fwXML
{

//------------------------------------------------------------------------------

ResectionXMLTranslator::ResectionXMLTranslator() {};

//------------------------------------------------------------------------------

ResectionXMLTranslator::~ResectionXMLTranslator() {};

//------------------------------------------------------------------------------

xmlNodePtr ResectionXMLTranslator::getXMLFrom( ::fwData::Object::sptr obj )
{

    ::fwData::Resection::sptr pResection = ::fwData::Resection::dynamicCast(obj);
    SLM_ASSERT("pResection not instanced", pResection);

    // create master node with className+id
    xmlNodePtr masterNode = XMLTranslatorHelper::MasterNode( obj );

    xmlNodePtr nameNode = XMLTH::newElement("name", pResection->getCRefName() );
    xmlAddChild( masterNode , nameNode );

    xmlNodePtr inputsNode =
            XMLTH::homogeneousContainerToXml("inputs", pResection->getCRefInputs().begin(),  pResection->getCRefInputs().end() );
    xmlAddChild( masterNode , inputsNode);
    xmlNodePtr outputsNode =
            XMLTH::homogeneousContainerToXml("outputs", pResection->getCRefOutputs().begin(),  pResection->getCRefOutputs().end() );
    xmlAddChild( masterNode , outputsNode);
    xmlNodePtr planelistNode = XMLTH::toXMLRecursive(pResection->getCRefPlaneList() );
    xmlAddChild( masterNode , planelistNode);

    xmlNodePtr isSafePartNode = XMLTH::newElement("isSafePart", pResection->getIsSafePart() );
    xmlAddChild( masterNode , isSafePartNode );
    xmlNodePtr isVisibleNode = XMLTH::newElement("isVisible", pResection->getIsVisible() );
    xmlAddChild( masterNode , isVisibleNode );

    xmlNodePtr isValidNode = XMLTH::newElement("isValid", pResection->getIsValid() );
    xmlAddChild( masterNode , isValidNode );

    return masterNode;
}

//------------------------------------------------------------------------------

void ResectionXMLTranslator::updateDataFromXML( ::fwData::Object::sptr toUpdate,  xmlNodePtr source)
{
    SLM_ASSERT("toUpdate not instanced", toUpdate); // object should exist
    //get its label
    ::fwData::Resection::sptr pResection = ::fwData::Resection::dynamicCast(toUpdate);
    pResection->getRefInputs().clear();
    pResection->getRefOutputs().clear();

    // If the point list is not empty
    if ( source->children != NULL )
    {
        xmlNodePtr elementNode = XMLParser::nextXMLElement( source->children );
        while ( elementNode )
        {
            std::string nodeName = (const char *) elementNode->name;
            if ( nodeName == "name" )
            {
                pResection->setName( XMLParser::getTextValue(elementNode));
            }
            else if ( nodeName == "isSafePart" )
            {
                std::string litteralValue = XMLParser::getTextValue(elementNode);
                pResection->setIsSafePart( ::boost::lexical_cast< bool >(  litteralValue ) );
            }
            else if ( nodeName == "isVisible" )
            {
                std::string litteralValue = XMLParser::getTextValue(elementNode);
                pResection->setIsVisible( ::boost::lexical_cast< bool >(  litteralValue ) );
            }
            else if ( nodeName == "isValid" )
            {
                std::string litteralValue = XMLParser::getTextValue(elementNode);
                pResection->setIsValid( ::boost::lexical_cast< bool >(  litteralValue ) );
            }
            else if ( nodeName == "inputs" )
            {
                if ( elementNode->children != NULL )
                {
                    xmlNodePtr inputsNode = XMLParser::nextXMLElement( elementNode->children );
                    while ( inputsNode )
                    {
                        // Load Reconstruction
                        ::fwData::Object::sptr valueObj;
                        valueObj = Serializer().ObjectsFromXml( inputsNode );

                        // Add reconstruction in the vector
                        SLM_ASSERT("valueObj not instanced", valueObj);
                        assert( ::fwData::Reconstruction::dynamicCast( valueObj ) );
                        pResection->getRefInputs().push_back( ::fwData::Reconstruction::dynamicCast( valueObj ) );

                        inputsNode = XMLParser::nextXMLElement( inputsNode->next );
                    }
                }
            }
            else if ( nodeName == "outputs" )
            {
                if ( elementNode->children != NULL )
                {
                    xmlNodePtr outputsNode = XMLParser::nextXMLElement( elementNode->children );
                    while ( outputsNode )
                    {
                        // Load Reconstruction
                        ::fwData::Object::sptr valueObj;
                        valueObj = Serializer().ObjectsFromXml( outputsNode );

                        // Add reconstruction in the vector
                        SLM_ASSERT("valueObj not instanced", valueObj);
                        assert( ::fwData::Reconstruction::dynamicCast( valueObj ) );
                        pResection->getRefOutputs().push_back( ::fwData::Reconstruction::dynamicCast( valueObj ) );

                        outputsNode = XMLParser::nextXMLElement( outputsNode->next );
                    }
                }
            }
            else if ( nodeName == "PlaneList" )
            {
                ::fwData::Object::sptr valueObj;
                valueObj = Serializer().ObjectsFromXml( elementNode );

                // Add PlaneList in the vector
                SLM_ASSERT("valueObj not instanced", valueObj);
                ::fwData::PlaneList::sptr planeList = ::fwData::PlaneList::dynamicCast( valueObj );
                SLM_ASSERT("planeList not instanced", planeList);
                pResection->setPlaneList( planeList );
            }

            // Element ++
            elementNode = XMLParser::nextXMLElement( elementNode->next );
        }
    }
}


}