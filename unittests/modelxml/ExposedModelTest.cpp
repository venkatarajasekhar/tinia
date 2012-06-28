/* Copyright STIFTELSEN SINTEF 2012
 * 
 * This file is part of the Tinia Framework.
 * 
 * The Tinia Framework is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * The Tinia Framework is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with the Tinia Framework.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <boost/test/unit_test.hpp>

#include <initializer_list>
#include <string>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <array>
#include <vector>
#include <ctime>
#include <cstring>
#include <memory>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>


#include "tinia/model/ExposedModel.hpp"
#include "tinia/model/ElementData.hpp"
#include "tinia/modelxml/XMLTransporter.hpp"
#include "tinia/modelxml/XMLReader.hpp"
#include "tinia/modelxml/utils.hpp"
#include "tinia/modelxml/XMLHandler.hpp"
#include "tinia/modelxml/ElementHandler.hpp"

#include "testutils.hpp"

#include <boost/algorithm/string.hpp>

using tinia::model::ElementData;
using tinia::modelxml::xpathQuery;

using namespace std;


BOOST_AUTO_TEST_SUITE( ExposedModelXml )

struct Fixture {
    Fixture() :
       model(new tinia::model::ExposedModel()),
       xmlHandler(model),
       elementHandler(model),

        xsd( "http://www.w3.org/2001/XMLSchema" ),
        xsi( "http://www.w3.org/2001/XMLSchema-instance" ),
        tns( "http://cloudviz.sintef.no/V1/model" )
    {

    }
    ~Fixture() {}

    std::shared_ptr<tinia::model::ExposedModel> model;
    tinia::modelxml::XMLTransporter xmlTransporter;
    // There is an xmlTransporter in model, but we are not allowed to access that one directly,
    // something we want to do for testing purposes.
    tinia::modelxml::XMLReader xmlReader;
    tinia::modelxml::XMLHandler xmlHandler;
   tinia::modelxml::ElementHandler elementHandler;
    const std::string xsd;
    const std::string xsi;
    const std::string tns;
};


/** Check the overall structure of the generated xml-document. */
BOOST_FIXTURE_TEST_CASE( getCompleteDocument,Fixture ) {

    xmlDocPtr doc = xmlHandler.getCompleteDocument();
    // Check that we have the following tree in the document
    // <ExposedModelUpdate>
    //   <StateSchema/>
    //   <State/>
    //   <GuiLayout/>
    // </ExposedModelUpdate>

    //std::vector<xmlChar*> puChilds = { (xmlChar*)"StateSchema", (xmlChar*)"State", (xmlChar*)"GuiLayout" };
    std::vector<xmlChar*> puChilds;
    puChilds.push_back( (xmlChar*)"StateSchema" );
    puChilds.push_back( (xmlChar*)"State" );
    puChilds.push_back( (xmlChar*)"GuiLayout" );

    auto model = doc->children;

    BOOST_REQUIRE( model != NULL );
    BOOST_CHECK( xmlStrEqual( model->name, (xmlChar*)( "ExposedModelUpdate" ) ) );

    auto child = model->children;

    for( auto i = 0u; i < puChilds.size(); ++i ) {
        BOOST_CHECK( xmlStrEqual( child->name, puChilds[i] ) );
        child = child->next;
    }

    BOOST_CHECK( child == 0 );

    xmlFreeDoc( doc );
}

void
print_xpath_nodes(xmlNodeSetPtr nodes, FILE* output) {
    xmlNodePtr cur;
    int size;
    int i;

    assert(output);
    size = (nodes) ? nodes->nodeNr : 0;

    fprintf(output, "Result (%d nodes):\n", size);
    for(i = 0; i < size; ++i) {
        assert(nodes->nodeTab[i]);

        if(nodes->nodeTab[i]->type == XML_NAMESPACE_DECL) {
            xmlNsPtr ns;

            ns = (xmlNsPtr)nodes->nodeTab[i];
            cur = (xmlNodePtr)ns->next;
            if(cur->ns) {
                fprintf(output, "= namespace \"%s\"=\"%s\" for node %s:%s\n",
                    ns->prefix, ns->href, cur->ns->href, cur->name);
            } else {
                fprintf(output, "= namespace \"%s\"=\"%s\" for node %s\n",
                    ns->prefix, ns->href, cur->name);
            }
        } else if(nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
            cur = nodes->nodeTab[i];
            if(cur->ns) {
                fprintf(output, "= element node \"%s:%s\"\n",
                    cur->ns->href, cur->name);
            } else {
                fprintf(output, "= element node \"%s\"\n",
                    cur->name);
            }
        } else {
            cur = nodes->nodeTab[i];
            fprintf(output, "= node \"%s\": type %d\n", cur->name, cur->type);
        }
    }
}

BOOST_FIXTURE_TEST_CASE( addUncontrainedInteger, Fixture ) {
    model->addElement( "isovalue", -4 );



    auto doc = xmlHandler.getCompleteDocument();
    //xmlSaveFormatFile( "-", doc, 1 );
    auto xpathContext = xmlXPathNewContext( doc );

    const std::string stateNodeXpathExpr = "/ExposedModelUpdate/State/isovalue";
    auto stateNodeXpathObject = xmlXPathEvalExpression( (xmlChar*)( stateNodeXpathExpr.c_str() ), xpathContext );

    // Check that we only have one isovalue-node
    BOOST_CHECK_EQUAL( stateNodeXpathObject->nodesetval->nodeNr, 1 );
    auto stateNode = stateNodeXpathObject->nodesetval->nodeTab[0];
    auto stateContent = xmlNodeGetContent( stateNode );
    BOOST_CHECK( xmlStrEqual( stateContent, (xmlChar*)( "-4" )));

    // Check that the stateSchema is defined properly
    xmlXPathRegisterNs( xpathContext, BAD_CAST "xsd", BAD_CAST xsd.c_str() );
    const std::string stateSchemaXpathExpr = "/ExposedModelUpdate/StateSchema/xsd:schema/xsd:element[@name='State']/xsd:complexType/xsd:all/xsd:element[@name='isovalue']";
    auto schemaNodeXpathObject = xmlXPathEvalExpression( (xmlChar*)( stateSchemaXpathExpr.c_str() ), xpathContext );
    BOOST_REQUIRE_EQUAL( schemaNodeXpathObject->nodesetval->nodeNr, 1 );

    auto elementNode = schemaNodeXpathObject->nodesetval->nodeTab[0];
    // Get the type attribute
    BOOST_REQUIRE( xmlHasProp( elementNode, BAD_CAST "type" ) );
    auto typeValue = xmlGetProp( elementNode, BAD_CAST "type" );
    BOOST_CHECK( xmlStrEqual( typeValue, BAD_CAST "xsd:integer" ) );

    // Clean up stuff
    xmlFree( stateContent );
    xmlXPathFreeObject( stateNodeXpathObject );
    xmlXPathFreeObject( schemaNodeXpathObject );

    xmlXPathFreeContext( xpathContext );
    xmlFreeDoc( doc );
}




BOOST_FIXTURE_TEST_CASE( addConstrainedInteger, Fixture ) {
    const int minValue = 2;
    const int maxValue = 42;
    model->addConstrainedElement( "timestep", 40, minValue, maxValue );
    auto doc = xmlHandler.getCompleteDocument();

    //xmlSaveFormatFile( "-", doc, 1 );

    auto node = xpathQuery( doc, "//xsd:element[@name='timestep']/xsd:simpleType" );
    BOOST_REQUIRE( node != 0 );

    // Check that we have a restriction subnode
    auto restriction = xpathQuery( doc, "//xsd:element[@name='timestep']/xsd:simpleType/xsd:restriction" );
    BOOST_REQUIRE( restriction != 0 );
    BOOST_REQUIRE( xmlHasProp( restriction, BAD_CAST "base" ) );

    // Check that the base="xsd:integer" attribute is set
    {
        auto baseType = xmlGetProp( restriction, BAD_CAST "base" );
        BOOST_CHECK( xmlStrEqual( baseType, BAD_CAST "xsd:integer" ) );
        xmlFree( baseType );
    }

    // Check that minInclusive has the right value
    {
        auto minInclusive = xpathQuery( doc, "//xsd:element[@name='timestep']/xsd:simpleType/xsd:restriction/xsd:minInclusive" );
        BOOST_REQUIRE( minInclusive != 0 );
        BOOST_REQUIRE( xmlHasProp( minInclusive, BAD_CAST "value" ) );

        int readMinValue = 0;
        tinia::modelxml::getXmlPropAsType( minInclusive, "value", readMinValue );

        BOOST_CHECK_EQUAL( readMinValue, minValue );
    }

    // Check that maxInclusive has the right value
    {
        auto maxInclusive = xpathQuery( doc, "//xsd:element[@name='timestep']/xsd:simpleType/xsd:restriction/xsd:maxInclusive" );
        BOOST_REQUIRE( maxInclusive != 0 );
        BOOST_REQUIRE( xmlHasProp( maxInclusive, BAD_CAST "value" ) );

        int readMaxValue = 0;
        tinia::modelxml::getXmlPropAsType( maxInclusive, "value", readMaxValue );
        BOOST_CHECK_EQUAL( readMaxValue, maxValue );

    }

    xmlFreeDoc( doc );
}

BOOST_FIXTURE_TEST_CASE( addConstrainedDoubleOutsideRange, Fixture ) {
    //double maxValue = 100.0d;
    //double minValue = -100.0d;
    double maxValue = 100.0;
    double minValue = -100.0;
    double d = maxValue + 1;

    BOOST_CHECK_THROW( model->addConstrainedElement( "aDouble", d, minValue, maxValue ), std::runtime_error );
}

struct AddMatrixFixture : public Fixture {
    AddMatrixFixture():
        matrixName( "projection" )
    {
        generate( myMatrix.begin(), myMatrix.end(), []()->int { static int i = 1; i++; return i; } );
        model->addMatrixElement( matrixName, myMatrix.data() );
    }

    const std::string matrixName;
    std::array<float, 16> myMatrix;
};

BOOST_FIXTURE_TEST_CASE( addMatrixElement, AddMatrixFixture ) {
    BOOST_CHECK( model->hasElement( matrixName ) );
    BOOST_CHECK_THROW( model->addMatrixElement( matrixName, myMatrix.data() ), std::runtime_error );
}

BOOST_FIXTURE_TEST_CASE( updateMatrixElement, AddMatrixFixture ) {
    std::vector<float> v0(16, -1.0f );
    std::vector<float> v1(16, 0.0f );

    model->updateMatrixValue( matrixName, v0.data() );

    model->getMatrixValue( matrixName, v1.data() );

    BOOST_CHECK_EQUAL_COLLECTIONS( v0.begin(), v0.end(), v1.begin(), v1.end() );

}

BOOST_FIXTURE_TEST_CASE( readMatrixElement, AddMatrixFixture ) {
    std::vector<float> v(16, -1.0f );

    model->getMatrixValue( matrixName, v.data() );

    BOOST_CHECK_EQUAL_COLLECTIONS( v.begin(), v.end(), myMatrix.begin(), myMatrix.end() );
    BOOST_CHECK_THROW( model->getMatrixValue( "notAMatrixName", v.data() ), std::runtime_error );
    model->addElement( "AnInt", -4 );
    BOOST_CHECK_THROW( model->getMatrixValue( "AnInt", v.data() ), std::runtime_error );
}

BOOST_FIXTURE_TEST_CASE( matrixSchemaDefinition, AddMatrixFixture ) {
    // Check that the schema-type is defined properly
    TestHelper( xmlHandler, [&]( xmlDocPtr doc, xmlNodePtr model ) {
        auto simpleType = xpathQuery( doc, "//xsd:element[@name='projection']" );
        BOOST_REQUIRE( simpleType != 0 );

        auto listType = xpathQuery( doc, "//xsd:element[@name='projection']/xsd:restriction/xsd:simpleType/xsd:list[@itemType='xsd:float']" );
        BOOST_REQUIRE( listType != 0 );

        auto length = xpathQuery( doc, "//xsd:element[@name='projection']/xsd:restriction/xsd:length[@value='16']" );
        BOOST_REQUIRE( length != 0 );
    }
    );
}

BOOST_FIXTURE_TEST_CASE( addMatrixElementXMLContents, AddMatrixFixture ) {
    TestHelper( xmlHandler, [&]( xmlDocPtr doc, xmlNodePtr model ) {
        auto projectionMatrix = xpathQuery( doc, "//projection" );
        BOOST_REQUIRE( projectionMatrix != 0 );

        std::string stringContents;
        tinia::modelxml::getXmlNodeContentAsType( projectionMatrix, stringContents );

        vector<string> splitted;
        boost::split( splitted, stringContents, boost::is_any_of(" ") );

        BOOST_REQUIRE_EQUAL( splitted.size(), myMatrix.size() );

        vector<float> readMatrix( splitted.size() );
        transform( splitted.begin(), splitted.end(), readMatrix.begin(), []( std::string s ) {
                  return boost::lexical_cast<float>( s ); }
                  );
        BOOST_CHECK_EQUAL_COLLECTIONS( myMatrix.begin(), myMatrix.end(), readMatrix.begin(), readMatrix.end() );
    } );
}




BOOST_FIXTURE_TEST_CASE( IncrementRevisionNumber, Fixture ) {
    std::vector<unsigned long> revisionNumbers(2);

    // Check that we have a sane revision number;
    {
        auto doc = xmlHandler.getCompleteDocument();
        auto model = doc->children;
        BOOST_REQUIRE( xmlHasProp( model, BAD_CAST "revision" ) );

        tinia::modelxml::getXmlPropAsType( model, "revision", revisionNumbers[0] );

        xmlFreeDoc( doc );
    }


    // Check that the revision number is updated
    {
        model->addElement( "isovalue", 42 );

        auto doc = xmlHandler.getCompleteDocument();
        auto model = doc->children;

        tinia::modelxml::getXmlPropAsType( model, "revision", revisionNumbers[1] );

        BOOST_CHECK_GT( revisionNumbers[1], revisionNumbers[0] );

        xmlFreeDoc( doc );
    }
}

BOOST_FIXTURE_TEST_CASE( IncrementRevisionNumberOnUpdate, Fixture ) {
    std::vector<unsigned long> revisionNumbers(2);
    const int startValue = 42;
    const std::string elementName = "isovalue";
    model->addElement( elementName, startValue );

    TestHelper( xmlHandler, [&]( xmlDocPtr doc, xmlNodePtr model ) {
        BOOST_REQUIRE( xmlHasProp( model, BAD_CAST "revision" ) );
        tinia::modelxml::getXmlPropAsType( model, "revision", revisionNumbers[0] );
    } );

    model->updateElement( elementName, 64 );

    TestHelper( xmlHandler, [&]( xmlDocPtr doc, xmlNodePtr model ) {
        BOOST_REQUIRE( xmlHasProp( model, BAD_CAST "revision" ) );
        tinia::modelxml::getXmlPropAsType( model, "revision", revisionNumbers[1] );
    } );

    BOOST_CHECK_GT( revisionNumbers[1], revisionNumbers[0] );
}


BOOST_FIXTURE_TEST_CASE( UpdateUnconstrainedElement, Fixture ) {
    const int startValue = 42;
    std::string elementName = "isovalue";
    model->addElement( elementName, startValue );

    TestHelper( xmlHandler, [&]( xmlDocPtr doc, xmlNodePtr model ) {
        auto node = xpathQuery( doc, "//" + elementName  );
        BOOST_REQUIRE( node != 0 );
        int readValue = 0;
        tinia::modelxml::getXmlNodeContentAsType( node, readValue );
        BOOST_CHECK_EQUAL( startValue, readValue );
    } );

    const int updatedValue = 64;
    model->updateElement( elementName, updatedValue );

    TestHelper( xmlHandler, [&]( xmlDocPtr doc, xmlNodePtr model ) {
        auto node = xpathQuery( doc, "//" + elementName );
        BOOST_REQUIRE( node != 0 );
        int readValue = 0;
        tinia::modelxml::getXmlNodeContentAsType( node, readValue );
        BOOST_CHECK_EQUAL( updatedValue, readValue );
    } );
}

BOOST_FIXTURE_TEST_CASE( UpdatedElementThatIsNotAdded, Fixture ) {
   BOOST_CHECK_THROW( model->updateElement( "sneakyVariable", 3.1514 ), std::runtime_error );
}

BOOST_FIXTURE_TEST_CASE( UpdateUnconstrainedElementWithOtherType, Fixture ) {
    std::string elementName = "isovalue";
    model->addElement( elementName, 42 );

    //BOOST_CHECK_THROW( model->updateElement( elementName, 3.1415d ), std::runtime_error );
    BOOST_CHECK_THROW( model->updateElement( elementName, 3.1415 ), std::runtime_error );
}

BOOST_FIXTURE_TEST_CASE( UpdateElementOutsideConstriant, Fixture ) {
    const int minValue = 2;
    const int maxValue = 42;
    const std::string elementName = "timestep";
    model->addConstrainedElement( elementName, 40, minValue, maxValue );

    BOOST_CHECK_THROW( model->updateElement( elementName, maxValue + 1 ), std::runtime_error );

}

BOOST_FIXTURE_TEST_CASE( AddElementTwice, Fixture ) {
    std::string elementName = "isovalue";
    BOOST_CHECK_NO_THROW( model->addElement( elementName, 42 ) );

    BOOST_CHECK_THROW( model->addElement( elementName, 42 ), std::runtime_error );
}

BOOST_FIXTURE_TEST_CASE( RemoveElement, Fixture ) {
    std::string elementName = "aName";
    int elementValue = 1;

    // We throw if element is not in model
    BOOST_CHECK_THROW( model->removeElement( elementName ), std::runtime_error );

    // Add and remove element
    model->addElement( elementName, elementValue );
    BOOST_CHECK_NO_THROW( model->removeElement( elementName ) );

    // Check that it is removed
    BOOST_CHECK_THROW( model->removeElement( elementName ), std::runtime_error );

}

BOOST_FIXTURE_TEST_CASE( GetIntegerElementValue, Fixture ) {
    std::string elementName = "aName";
    const int elementValue = 1;
    int readValue = 0;
    model->addElement( elementName, elementValue );

    model->getElementValue( elementName, readValue );
    BOOST_CHECK_EQUAL( readValue, elementValue );

    BOOST_CHECK_THROW( model->getElementValue( "aNameNotThere", readValue ), std::runtime_error );
}


/** Test that the static_assert macro i working. Uncommenting this line should trigger a compile time failure. */
BOOST_FIXTURE_TEST_CASE( checkCompileFaile, Fixture ) {

    /* Uncomment the following to test. */
    //    int i = 0;
    //    int* ip = &i;
    // model->addElement( "intpointer", ip );
}



BOOST_FIXTURE_TEST_CASE( parseExposedModelDocument, Fixture )
{
    // 1. Generate a model

    int    test1 = 123;
    double test2 = 123.456;

    model->addElement( std::string("test1"), test1 );
    model->addElement( std::string("test2"), test2 );

    xmlDocPtr doc = xmlHandler.getCompleteDocument();
    // xmlSaveFormatFile( "-", doc, 1 );

    // 2. Modify the values, so that we can test 'parseDocument' properly afterward

    model->updateElement( std::string("test1"), test1+1 );
    model->updateElement( std::string("test2"), test2+1.0 );

    // 3. Parse the model and obtain the original state

    xmlReader.parseDocument(doc, elementHandler);;

    // 4. Check that the elements in that second state are equal to the original ones.

    int a;
    double b;
    model->getElementValue( std::string("test1"), a );
    // printf("a=%d\n", a);
    model->getElementValue( std::string("test2"), b );
    // printf("b=%f\n", b);

    BOOST_CHECK_EQUAL( test1, a );
    BOOST_CHECK_EQUAL( test2, b );
}



BOOST_FIXTURE_TEST_CASE( parseExposedModelDocumentForEmptyDoc, Fixture )
{
  xmlDocPtr doc = xmlHandler.getCompleteDocument();
  // xmlSaveFormatFile( "-", doc, 1 );
  xmlReader.parseDocument(doc, elementHandler);;
  BOOST_CHECK_EQUAL( true, true );
}




BOOST_FIXTURE_TEST_CASE( HasElement, Fixture ) {
    const std::string elementName = "render_mode";
    std::string elementValue = "foo";

   BOOST_CHECK( !model->hasElement(  elementName ) );

   model->addElement( elementName, elementValue );
   BOOST_CHECK( model->hasElement( elementName ) );
}

BOOST_FIXTURE_TEST_CASE( AddStringWithRestriction, Fixture ) {
    const std::string elementName = "render_mode";
    std::vector<std::string> restriction({ "points", "wireframe", "solid" });
    model->addElementWithRestriction<std::string>( elementName, "points", restriction );
    BOOST_REQUIRE( model->hasElement( elementName ) );

    BOOST_CHECK_NO_THROW( model->updateElement( elementName, "wireframe" ) );
    BOOST_CHECK_THROW( model->updateElement( elementName, "texture" ), std::runtime_error );

    std::string texture( "texture" );
    BOOST_CHECK_THROW( model->updateElement( elementName, texture ), std::runtime_error );
}

BOOST_FIXTURE_TEST_CASE( AddStringWithRestrictionNotInList, Fixture ) {
  std::vector<std::string> restrictions({"bar", "gaz"});
    BOOST_CHECK_THROW( model->addElementWithRestriction<std::string>( "foobar", "foo", restrictions ), std::runtime_error );
}

BOOST_FIXTURE_TEST_CASE( StringRestrictionXSD, Fixture ) {
    const std::string elementName = "render_mode";
    std::vector<std::string> restrictionList({ "points", "wireframe", "solid" });
    model->addElementWithRestriction<std::string>( elementName, "points", restrictionList );

    TestHelper( xmlHandler, [&]( xmlDocPtr doc, xmlNodePtr model ) {
        auto node = xpathQuery( doc, "//xsd:element[@name='render_mode']");
        BOOST_REQUIRE( node != 0 );

        node = xpathQuery( doc, "//xsd:element[@name='render_mode']/xsd:simpleType/xsd:restriction");
        BOOST_REQUIRE( node != 0 );
#if 0
        std::for_each( restrictionList.begin(), restrictionList.end(), [&]( std::string restriction ) {
            std::string query = "//xsd:element[@name='render_mode']/xsd:simpleType/xsd:restriction/xsd:enumeration[@value='";
            query += restriction;
            query += "']";
            node = xpathQuery( doc, query ) ;
            BOOST_CHECK( node != 0 );
        }
        );
#endif

    } );

}

struct AnnotationFixture : public Fixture {
    AnnotationFixture() :
        elementName( "timestep" ),
        annotation( "Time step" ) {}

    const std::string elementName;
    const std::string annotation;
};

BOOST_FIXTURE_TEST_CASE( AddAnnotation, AnnotationFixture ) {
    // Check that we throw if we add an anotation that is not there.
    BOOST_CHECK_THROW( model->addAnnotation( elementName, annotation ), std::runtime_error );

    model->addElement( elementName, 10 );
    BOOST_CHECK_NO_THROW( model->addAnnotation( elementName, annotation ) );
}

BOOST_FIXTURE_TEST_CASE( AddAnnotationResult, AnnotationFixture ) {
    model->addElement( elementName, 10 );
    model->addAnnotation( elementName, annotation );

     TestHelper( xmlHandler, [&]( xmlDocPtr doc, xmlNodePtr model ) {
        auto node = xpathQuery( doc, "//xsd:element[@name='timestep']/xsd:annotation/xsd:documentation[@xml:lang='en']");
        BOOST_REQUIRE( node != 0 );

        std::string readAnnotation;
        tinia::modelxml::getXmlNodeContentAsType( node, readAnnotation );
        BOOST_CHECK_EQUAL( readAnnotation, annotation );
    } );
}

struct MultiLangAnnotationFixture : public Fixture {
    MultiLangAnnotationFixture() :
        elementName( "timestep" ),
        elementValue( 10 )
    {}
//        annotation( "Time step" ) {}

    const std::string elementName;
    int elementValue;
    //const std::string annotation;
};


BOOST_FIXTURE_TEST_CASE( getExposedModelUpdate, Fixture )
{
    int test1 = 123;
    double test2 = 123.456;
    model->addElement( std::string("test1"), test1 );

    const int rev_0 = model->getRevisionNumber();
    // printf("rev_0 = %d\n", rev_0);

    const double old_test2 = test2;

    model->addElement( std::string("test2"), test2 );
    //model->updateElement( std::string("test1"), test1+1 );
    //model->updateElement( std::string("test2"), test2+1.0 );

    vector<char> testBuffer(1024*1024, 0);
    size_t bytes_written = xmlHandler.getExposedModelUpdate(&testBuffer[0], testBuffer.size(), rev_0 );
    // printf("bytes written: %lu\n", bytes_written);

    // For this update, only 'test2' should be included. We check this by changing the state and then use
    // the xml-delta to restore the state. Then 'test1' should have the new value, and 'test2' the old one.

    const int new_test1 = test1 + 1;
    const double new_test2 = test2 + 1.0;
    model->updateElement( std::string("test1"), new_test1 );
    model->updateElement( std::string("test2"), new_test2 );

    xmlDocPtr doc = xmlTransporter.readXMLfromBuffer(&testBuffer[0], bytes_written);
    // xmlSaveFormatFile( "-", doc, 1 );

    // Undoing the last two updates
    xmlReader.parseDocument(doc, elementHandler);;

    // Inspecting and checking the state
    model->getElementValue( std::string("test1"), test1 );
    model->getElementValue( std::string("test2"), test2 );
    // printf("test1=%d\n", test1);
    // printf("test2=%f\n", test2);

    BOOST_CHECK_EQUAL( test1, new_test1 );
    BOOST_CHECK_EQUAL( test2, old_test2 );
}


BOOST_FIXTURE_TEST_CASE( getExposedModelUpdate2, Fixture )
{
    // Testing an empty update

    int test1 = 123;
    double test2 = 123.456;
    model->addElement( std::string("test1"), test1 );

    model->addElement( std::string("test2"), test2 );
    //model->updateElement( std::string("test1"), test1+1 );
    //model->updateElement( std::string("test2"), test2+1.0 );

    vector<char> testBuffer(1024*1024, 0);
    size_t bytes_written = xmlHandler.getExposedModelUpdate(&testBuffer[0], testBuffer.size(), model->getRevisionNumber() );
    //printf("bytes written: %lu\n", bytes_written);

    const int new_test1 = test1 + 1;
    const double new_test2 = test2 + 1.0;
    model->updateElement( std::string("test1"), new_test1 );
    model->updateElement( std::string("test2"), new_test2 );

    if (bytes_written>0) {
        // For this update, only 'test2' should be included. We check this by changing the state and then use
        // the xml-delta to restore the state. Then 'test1' should have the new value, and 'test2' the old one.

        xmlDocPtr doc = xmlTransporter.readXMLfromBuffer(&testBuffer[0], bytes_written);
        //xmlSaveFormatFile( "-", doc, 1 );

        // Undoing the last two updates. This should not do anything now.
        xmlReader.parseDocument(doc, elementHandler);;
    }

    // Inspecting and checking the state
    model->getElementValue( std::string("test1"), test1 );
    model->getElementValue( std::string("test2"), test2 );
    // printf("test1=%d\n", test1);
    // printf("test2=%f\n", test2);

    BOOST_CHECK_EQUAL( test1, new_test1 );
    BOOST_CHECK_EQUAL( test2, new_test2 );
}


BOOST_FIXTURE_TEST_CASE( getExposedModelUpdate3, Fixture )
{
    // Testing for memory leaks. This test will likely explode if the number of iterations is sufficiently high,
    // and there is a memory leak.

    const int iterations = 10000;

    int    test1 = 123;
    double test2 = 123.456;
    const int    old_test1 = test1;
    const double old_test2 = test2;
    model->addElement( std::string("test1"), test1 );
    model->addElement( std::string("test2"), test2 );

    vector<char> testBuffer(1024*1024, 0);
    size_t bytes_written = xmlHandler.getExposedModelUpdate(&testBuffer[0], testBuffer.size(), 0 );

    // const double t = clock()/double(CLOCKS_PER_SEC);
    for (int i=0; i<iterations; i++) {
        test1 = test1 + 1;
        test2 = test2 + 1.0;
        model->updateElement( std::string("test1"), test1 );
        model->updateElement( std::string("test2"), test2 );

        xmlDocPtr doc = xmlTransporter.readXMLfromBuffer(&testBuffer[0], bytes_written);
        // xmlSaveFormatFile( "-", doc, 1 );

        // Undoing the last updates. After this, both test1 and test2 should have their old values.
        xmlReader.parseDocument(doc, elementHandler);;
        xmlFreeDoc(doc);

        // Inspecting and checking the state
        model->getElementValue( std::string("test1"), test1 );
        model->getElementValue( std::string("test2"), test2 );
        // printf("test1=%d\n", test1);
        // printf("test2=%f\n", test2);

        BOOST_CHECK_EQUAL( test1, old_test1 );
        BOOST_CHECK_EQUAL( test2, old_test2 );
    }
    // const double t2 = clock()/double(CLOCKS_PER_SEC);
    //printf("Elapsed time: %.2f s\n", (t2-t) );
}


BOOST_FIXTURE_TEST_CASE( getExposedModelUpdate4, Fixture )
{
    // Testing for memory leaks. This test will likely explode if the number of iterations is sufficiently high,
    // and there is a memory leak.
    // Larger document

    const int iterations = 10;
    const int vec_length = 100;

    int    test1 = 123;
    double test2 = 123.456;
    const int    old_test1 = test1;
    const double old_test2 = test2;
    model->addElement( std::string("test1"), test1 );
    model->addElement( std::string("test2"), test2 );
    vector<double> test_vec(vec_length, 3.1415);
    for (int i=0; i<int(test_vec.size()); i++) {
        stringstream ss;
        ss << "var_" << i;
        model->addElement(ss.str(), test_vec[i]);
    }

    vector<char> testBuffer(1024*1024, 0);
    size_t bytes_written = xmlHandler.getExposedModelUpdate(&testBuffer[0], testBuffer.size(), 0 );

    // const double t = clock()/double(CLOCKS_PER_SEC);
    for (int i=0; i<iterations; i++) {
        test1 = test1 + 1;
        test2 = test2 + 1.0;
        model->updateElement( std::string("test1"), test1 );
        model->updateElement( std::string("test2"), test2 );

        xmlDocPtr doc = xmlTransporter.readXMLfromBuffer(&testBuffer[0], bytes_written);
        //xmlSaveFormatFile( "-", doc, 1 );

        // Undoing the last updates. After this, both test1 and test2 should have their old values.
        xmlReader.parseDocument(doc, elementHandler);;
        xmlFreeDoc(doc);

        // Inspecting and checking the state
        model->getElementValue( std::string("test1"), test1 );
        model->getElementValue( std::string("test2"), test2 );
        // printf("test1=%d\n", test1);
        // printf("test2=%f\n", test2);

        BOOST_CHECK_EQUAL( test1, old_test1 );
        BOOST_CHECK_EQUAL( test2, old_test2 );
    }
    // const double t2 = clock()/double(CLOCKS_PER_SEC);
   // printf("Elapsed time: %.2f s\n", (t2-t) );
}








BOOST_FIXTURE_TEST_CASE( getExposedModelUpdateForMatrix, Fixture )
{
    int    test1 = 123;
    double test2 = 123.456;
    vector<float> mat(16);
    for (int i=0; i<16; i++)
        mat[i] = float(i);

    model->addElement( std::string("test1"), test1 );
    model->addElement( std::string("test2"), test2 );
    //const int         old_test1 = test1;
    //const double      old_test2 = test2;

    model->addMatrixElement( std::string("mat"), &mat[0] );
    const vector<float> old_mat = mat;

    // For this update, only 'mat' should be included, since that was the last
    // change to the state.
    vector<char> testBuffer(1024*1024, 0);
    size_t bytes_written = xmlHandler.getExposedModelUpdate(&testBuffer[0],
                                                     testBuffer.size(),
                                                     model->getRevisionNumber()-1);
    xmlDocPtr doc = xmlTransporter.readXMLfromBuffer(&testBuffer[0], bytes_written);
    //xmlSaveFormatFile( "-", doc, 1 );

    mat[1] = 999.0;
    const vector<float> new_mat = mat;
    model->updateMatrixValue( std::string("mat"), &mat[0] );

    const int         new_test1 = test1 + 1;
    model->updateElement( std::string("test1"), new_test1 );

    const double      new_test2 = test2 + 1.0;
    model->updateElement( std::string("test2"), new_test2 );

    // Undoing the last update, so that we get back the old value where 999 was put.
    xmlReader.parseDocument(doc, elementHandler);;

    // Inspecting and checking the state
    model->getElementValue( std::string("test1"), test1 );
    model->getElementValue( std::string("test2"), test2 );
    model->getMatrixValue( std::string("mat"), &mat[0] );

    BOOST_CHECK_EQUAL( test1, new_test1 );
    BOOST_CHECK_EQUAL( test2, new_test2 );
    BOOST_CHECK_EQUAL( mat[1], old_mat[1] );
}


BOOST_FIXTURE_TEST_CASE( updateState, Fixture )
{
    int test1 = 123;
    double test2 = 123.456;
    model->addElement( std::string("test1"), test1 );

    const int rev_0 = model->getRevisionNumber();
    //printf("rev_0 = %d\n", rev_0);

    const double old_test2 = test2;

    model->addElement( std::string("test2"), test2 );
    //model->updateElement( std::string("test1"), test1+1 );
    //model->updateElement( std::string("test2"), test2+1.0 );

    vector<char> testBuffer(1024*1024, 0);
    size_t bytes_written = xmlHandler.getExposedModelUpdate(&testBuffer[0], testBuffer.size(), rev_0 );
    // printf("bytes written: %lu\n", bytes_written);

    // For this update, only 'test2' should be included. We check this by changing the state and then use
    // the xml-delta to restore the state. Then 'test1' should have the new value, and 'test2' the old one.

    const int    new_test1 = test1 + 1;
    const double new_test2 = test2 + 1.0;
    model->updateElement( std::string("test1"), new_test1 );
    model->updateElement( std::string("test2"), new_test2 );

    xmlDocPtr doc = xmlTransporter.readXMLfromBuffer(&testBuffer[0], bytes_written);
    //xmlSaveFormatFile( "-", doc, 1 );

    // doc should now contain the test2-addition, i.e. the old_test2 value.

    // Undoing the last two updates. There are two methods we can use, 1) parseDocument, or 2) updateState,
    // which does the same thing after creating the document with readXMLfromBuffer(...).

        xmlReader.parseDocument(doc, elementHandler);


    // Inspecting and checking the state
    model->getElementValue( std::string("test1"), test1 );
    model->getElementValue( std::string("test2"), test2 );
    // printf("test1=%d\n", test1);
    // printf("test2=%f\n", test2);

    BOOST_CHECK_EQUAL( test1, new_test1 );
    BOOST_CHECK_EQUAL( test2, old_test2 );
}


BOOST_AUTO_TEST_SUITE_END()





































