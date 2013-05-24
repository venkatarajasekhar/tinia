#include <boost/test/unit_test.hpp>
#include "tinia/model.hpp"
#include "tinia/model/impl/xml/PugiXMLReader.hpp"

BOOST_AUTO_TEST_SUITE(PugiXMLReaderTest)

BOOST_AUTO_TEST_CASE(AssertCorrectConstruction) {
    boost::shared_ptr<tinia::model::ExposedModel>
            model(new tinia::model::ExposedModel);

    BOOST_CHECK_NO_THROW(tinia::model::impl::xml::PugiXMLReader reader(model));
}

struct PugiXMLReaderTestFixture {
    boost::shared_ptr<tinia::model::ExposedModel> model;
    tinia::model::impl::xml::PugiXMLReader reader;

    PugiXMLReaderTestFixture()
        : model(new tinia::model::ExposedModel),
        reader(model) {}
};

BOOST_FIXTURE_TEST_CASE(ReadFaultyDocumentAssertThrow, PugiXMLReaderTestFixture) {
    std::string faultyDocument = "<?xml><start>";

    BOOST_CHECK_THROW(reader.readState(faultyDocument), std::exception);

}

BOOST_FIXTURE_TEST_CASE(ReadDocumentWithoutStateAssertThrow, PugiXMLReaderTestFixture) {
    std::string incorrectDocument = "<?xml version=\"1.0\"?>"
            "<someothertag />";

    BOOST_CHECK_THROW(reader.readState(incorrectDocument), std::exception);
}

BOOST_FIXTURE_TEST_CASE(ReadCorrectDocumentAssertNoThrow, PugiXMLReaderTestFixture) {
    // First we need to add a variable to the exposedmodel to make this correct
    model->addElement("key", 0);

    std::string correctDocument = "<?xml version=\"1.0\"?>"
                                   "<ExposedModelUpdate xmlns=\"http://cloudviz.sintef.no/V1/model\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://cloudviz.sintef.no/V1/model/ExposedModelUpdateSchema.xsd\" revision=\"10456\">"
                                   "    <StateSchema xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">"
                                   "        <xsd:schema>"
                                   "            <xsd:element name=\"State\">"
                                   "                 <xsd:complexType>"
                                   "                      <xsd:all>"
                                   "                          <xsd:element name=\"key\" type=\"xsd:integer\" />"
                                   "                      </xsd:all>"
                                   "                  </xsd:complexType>"
                                   "            </xsd:element>"
                                   "        </xsd:schema>"
                                   "   </StateSchema>"
                                   "   <State>"
                                   "       <key>0</key>"
                                   "   </State>"
                                   "   <GuiLayout>"
                                   "   </GuiLayout>"
                                   "</ExposedModelUpdate>";

    BOOST_CHECK_NO_THROW(reader.readState(correctDocument));
}

BOOST_FIXTURE_TEST_CASE(UpdateInt, PugiXMLReaderTestFixture) {
    // First we need to add a variable to the exposedmodel to make this correct
    model->addElement("key", 0);

    std::string correctDocument = "<?xml version=\"1.0\"?>"
                                   "<ExposedModelUpdate xmlns=\"http://cloudviz.sintef.no/V1/model\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://cloudviz.sintef.no/V1/model/ExposedModelUpdateSchema.xsd\" revision=\"10456\">"
                                   "    <StateSchema xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">"
                                   "        <xsd:schema>"
                                   "            <xsd:element name=\"State\">"
                                   "                 <xsd:complexType>"
                                   "                      <xsd:all>"
                                   "                          <xsd:element name=\"key\" type=\"xsd:integer\" />"
                                   "                      </xsd:all>"
                                   "                  </xsd:complexType>"
                                   "            </xsd:element>"
                                   "        </xsd:schema>"
                                   "   </StateSchema>"
                                   "   <State>"
                                   "       <key>1</key>"
                                   "   </State>"
                                   "   <GuiLayout>"
                                   "   </GuiLayout>"
                                   "</ExposedModelUpdate>";

    BOOST_CHECK_NO_THROW(reader.readState(correctDocument));
    BOOST_CHECK_EQUAL(1, model->getElementValue<int>("key"));
}

BOOST_FIXTURE_TEST_CASE(UpdateString, PugiXMLReaderTestFixture) {
    // First we need to add a variable to the exposedmodel to make this correct
    model->addElement("key", std::string("value"));

    std::string correctDocument = "<?xml version=\"1.0\"?>"
                                   "<ExposedModelUpdate xmlns=\"http://cloudviz.sintef.no/V1/model\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://cloudviz.sintef.no/V1/model/ExposedModelUpdateSchema.xsd\" revision=\"10456\">"
                                   "    <StateSchema xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">"
                                   "        <xsd:schema>"
                                   "            <xsd:element name=\"State\">"
                                   "                 <xsd:complexType>"
                                   "                      <xsd:all>"
                                   "                          <xsd:element name=\"key\" type=\"xsd:string\" />"
                                   "                      </xsd:all>"
                                   "                  </xsd:complexType>"
                                   "            </xsd:element>"
                                   "        </xsd:schema>"
                                   "   </StateSchema>"
                                   "   <State>"
                                   "       <key>new value</key>"
                                   "   </State>"
                                   "   <GuiLayout>"
                                   "   </GuiLayout>"
                                   "</ExposedModelUpdate>";

    BOOST_CHECK_NO_THROW(reader.readState(correctDocument));
    BOOST_CHECK_EQUAL(std::string("new value"), model->getElementValue<std::string>("key"));
}

BOOST_FIXTURE_TEST_CASE(UpdateStringAndInt, PugiXMLReaderTestFixture) {
    // First we need to add a variable to the exposedmodel to make this correct
    model->addElement("stringKey", std::string("value"));
    model->addElement("intKey", 1);
    std::string correctDocument = "<?xml version=\"1.0\"?>"
                                   "<ExposedModelUpdate xmlns=\"http://cloudviz.sintef.no/V1/model\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://cloudviz.sintef.no/V1/model/ExposedModelUpdateSchema.xsd\" revision=\"10456\">"
                                   "    <StateSchema xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">"
                                   "        <xsd:schema>"
                                   "            <xsd:element name=\"State\">"
                                   "                 <xsd:complexType>"
                                   "                      <xsd:all>"
                                   "                          <xsd:element name=\"stringKey\" type=\"xsd:string\" />"
                                   "                           <xsd:element name=\"intKey\" type=\"int\" />"
                                   "                      </xsd:all>"
                                   "                  </xsd:complexType>"
                                   "            </xsd:element>"
                                   "        </xsd:schema>"
                                   "   </StateSchema>"
                                   "   <State>"
                                   "       <stringKey>new value</stringKey>"
                                   "       <intKey>4</intKey>"
                                   "   </State>"
                                   "   <GuiLayout>"
                                   "   </GuiLayout>"
                                   "</ExposedModelUpdate>";

    BOOST_CHECK_NO_THROW(reader.readState(correctDocument));
    BOOST_CHECK_EQUAL(std::string("new value"),
                      model->getElementValue<std::string>("stringKey"));
    BOOST_CHECK_EQUAL(4,
                      model->getElementValue<int>("intKey"));
}

BOOST_FIXTURE_TEST_CASE(UpdateViewer, PugiXMLReaderTestFixture) {
    model->addElement("viewer", tinia::model::Viewer());

    // Just making sure we actually update.
    BOOST_CHECK_NE(10, model->getElementValue<tinia::model::Viewer>("viewer").width);
    BOOST_CHECK_NE(30, model->getElementValue<tinia::model::Viewer>("viewer").projectionMatrix[0]);
    std::string documentWithViewer = "<?xml version=\"1.0\"?>"
                                   "<ExposedModelUpdate xmlns=\"http://cloudviz.sintef.no/V1/model\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://cloudviz.sintef.no/V1/model/ExposedModelUpdateSchema.xsd\" revision=\"10456\">"
                                   "    <StateSchema xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">"
                                   "        <xsd:schema>"
                                   "            <xsd:element name=\"State\">"
                                   "                 <xsd:complexType>"
                                   "                      <xsd:all>"
            "<xsd:complexType name=\"viewer\">"
                "<xsd:sequence>"
                    "<xsd:element name=\"width\" type=\"xsd:integer\"/>"
                    "<xsd:element name=\"height\" type=\"xsd:integer\"/>"
                    "<xsd:element name=\"projection\">"
                        "<xsd:restriction>"
                            "<xsd:simpleType>"
                                "<xsd:list itemType=\"xsd:float\"/>"
                            "</xsd:simpleType>"
                            "<xsd:length value=\"16\"/>"
                        "</xsd:restriction>"
                    "</xsd:element>"
                    "<xsd:element name=\"modelview\">"
                        "<xsd:restriction>"
                            "<xsd:simpleType>"
                                "<xsd:list itemType=\"xsd:float\"/>"
                            "</xsd:simpleType>"
                            "<xsd:length value=\"16\"/>"
                        "</xsd:restriction>"
                    "</xsd:element>"
                    "<xsd:element name=\"timestamp\" type=\"xsd:double\"/>"
                    "<xsd:element name=\"sceneView\" type=\"xsd:string\"/>"
                "</xsd:sequence>"
            "</xsd:complexType>"
                                   "                      </xsd:all>"
                                   "                  </xsd:complexType>"
                                   "            </xsd:element>"
                                   "        </xsd:schema>"
                                   "   </StateSchema>"
                                   "   <State>"
                                    "<viewer>"
                                              "                          <width>10</width>"
                                              "                          <height>782</height>"
                                              "                          <projection>30 0 0 0 0 1 0 0 0 0 -1.0202020406723022 -1 0 0 -18.5638427734375 0</projection>"
                                              "<modelview>0.7354713678359985 -0.6516221761703491 0.18566226959228516 0 0.2348913997411728 0.502234697341919 0.832217812538147 0 -0.6355375647544861 -0.5684619545936584 0.5224396586418152 0 -65.47185516357422 -58.561859130859375 -436.5816650390625 1</modelview>"
                                              "<timestamp>20</timestamp>"
                                           "<sceneView>---oooOOOooo---</sceneView>"
                       "</viewer>"
                                   "   </State>"
                                   "   <GuiLayout>"
                                   "   </GuiLayout>"
                                   "</ExposedModelUpdate>";

    reader.readState(documentWithViewer);
    BOOST_CHECK_EQUAL(10, model->getElementValue<tinia::model::Viewer>("viewer").width);
    BOOST_CHECK_EQUAL(30, model->getElementValue<tinia::model::Viewer>("viewer").projectionMatrix[0]);
}

BOOST_FIXTURE_TEST_CASE(SchemaReadAssertNoThrow, PugiXMLReaderTestFixture) {
    std::string correctDocument = "<?xml version=\"1.0\"?>"
                                   "<ExposedModelUpdate xmlns=\"http://cloudviz.sintef.no/V1/model\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://cloudviz.sintef.no/V1/model/ExposedModelUpdateSchema.xsd\" revision=\"10456\">"
                                   "    <StateSchema xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">"
                                   "        <xsd:schema>"
                                   "            <xsd:element name=\"State\">"
                                   "                 <xsd:complexType>"
                                   "                      <xsd:all>"
            "<xsd:complexType name=\"viewer\">"
                "<xsd:sequence>"
                    "<xsd:element name=\"width\" type=\"xsd:integer\"/>"
                    "<xsd:element name=\"height\" type=\"xsd:integer\"/>"
                    "<xsd:element name=\"projection\">"
                        "<xsd:restriction>"
                            "<xsd:simpleType>"
                                "<xsd:list itemType=\"xsd:float\"/>"
                            "</xsd:simpleType>"
                            "<xsd:length value=\"16\"/>"
                        "</xsd:restriction>"
                    "</xsd:element>"
                    "<xsd:element name=\"modelview\">"
                        "<xsd:restriction>"
                            "<xsd:simpleType>"
                                "<xsd:list itemType=\"xsd:float\"/>"
                            "</xsd:simpleType>"
                            "<xsd:length value=\"16\"/>"
                        "</xsd:restriction>"
                    "</xsd:element>"
                    "<xsd:element name=\"timestamp\" type=\"xsd:double\"/>"
                    "<xsd:element name=\"sceneView\" type=\"xsd:string\"/>"
                "</xsd:sequence>"
            "</xsd:complexType>"
                                   "                      </xsd:all>"
                                   "                  </xsd:complexType>"
                                   "            </xsd:element>"
                                   "        </xsd:schema>"
                                   "   </StateSchema>"
                                   "   <State>"
                                    "<viewer>"
                                              "                          <width>10</width>"
                                              "                          <height>782</height>"
                                              "                          <projection>30 0 0 0 0 1 0 0 0 0 -1.0202020406723022 -1 0 0 -18.5638427734375 0</projection>"
                                              "<modelview>0.7354713678359985 -0.6516221761703491 0.18566226959228516 0 0.2348913997411728 0.502234697341919 0.832217812538147 0 -0.6355375647544861 -0.5684619545936584 0.5224396586418152 0 -65.47185516357422 -58.561859130859375 -436.5816650390625 1</modelview>"
                                              "<timestamp>20</timestamp>"
                                           "<sceneView>---oooOOOooo---</sceneView>"
                       "</viewer>"
                                   "   </State>"
                                   "   <GuiLayout>"
                                   "   </GuiLayout>"
                                   "</ExposedModelUpdate>";

    BOOST_CHECK_NO_THROW(reader.readSchema(correctDocument));
}

BOOST_FIXTURE_TEST_CASE(SchemaReadAssertThrowOnWithoutSchema, PugiXMLReaderTestFixture) {
    std::string incorrectDocument = "<?xml version=\"1.0\"?>"
                                   "<ExposedModelUpdate xmlns=\"http://cloudviz.sintef.no/V1/model\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://cloudviz.sintef.no/V1/model/ExposedModelUpdateSchema.xsd\" revision=\"10456\">"
                                   "    <StateSchema xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">"
                                   "   </StateSchema>"
                                   "   <State>"
                                    "<viewer>"
                                              "                          <width>10</width>"
                                              "                          <height>782</height>"
                                              "                          <projection>30 0 0 0 0 1 0 0 0 0 -1.0202020406723022 -1 0 0 -18.5638427734375 0</projection>"
                                              "<modelview>0.7354713678359985 -0.6516221761703491 0.18566226959228516 0 0.2348913997411728 0.502234697341919 0.832217812538147 0 -0.6355375647544861 -0.5684619545936584 0.5224396586418152 0 -65.47185516357422 -58.561859130859375 -436.5816650390625 1</modelview>"
                                              "<timestamp>20</timestamp>"
                                           "<sceneView>---oooOOOooo---</sceneView>"
                       "</viewer>"
                                   "   </State>"
                                   "   <GuiLayout>"
                                   "   </GuiLayout>"
                                   "</ExposedModelUpdate>";

    BOOST_CHECK_THROW(reader.readSchema(incorrectDocument), std::exception);
}

BOOST_FIXTURE_TEST_CASE(SchemaReadAssertThrowOnWithFaultyDocument, PugiXMLReaderTestFixture) {
    std::string incorrectDocument = "<?xml version=\"1.0\"?>"
                                   "   </State>"
                                   "   <GuiLayout>"
                                   "   </GuiLayout>"
                                   "</ExposedModelUpdate>";

    BOOST_CHECK_THROW(reader.readSchema(incorrectDocument), std::exception);
}

BOOST_FIXTURE_TEST_CASE(SchemaReadCheckSimpleDocument, PugiXMLReaderTestFixture) {
    std::string correctDocument = "<?xml version=\"1.0\"?>"
                                   "<ExposedModelUpdate xmlns=\"http://cloudviz.sintef.no/V1/model\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://cloudviz.sintef.no/V1/model/ExposedModelUpdateSchema.xsd\" revision=\"10456\">"
                                   "    <StateSchema xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">"
                                   "        <xsd:schema>"
                                   "            <xsd:element name=\"State\">"
                                   "                 <xsd:complexType>"
                                   "                      <xsd:all>"
                                   "                          <xsd:element name=\"stringKey\" type=\"xsd:string\" />"
                                   "                           <xsd:element name=\"intKey\" type=\"xsd:integer\" />"
                                   "                      </xsd:all>"
                                   "                  </xsd:complexType>"
                                   "            </xsd:element>"
                                   "        </xsd:schema>"
                                   "   </StateSchema>"
                                   "   <State>"
                                   "       <stringKey>new value</stringKey>"
                                   "       <intKey>4</intKey>"
                                   "   </State>"
                                   "   <GuiLayout>"
                                   "   </GuiLayout>"
                                   "</ExposedModelUpdate>";

    BOOST_CHECK_NO_THROW(reader.readSchema(correctDocument));
    BOOST_CHECK_NO_THROW(reader.readState(correctDocument));

    BOOST_CHECK_EQUAL("new value", model->getElementValue<std::string>("stringKey"));
    BOOST_CHECK_EQUAL(4, model->getElementValue<int>("intKey"));
}

BOOST_FIXTURE_TEST_CASE(SchemaReadCheckSimpleDocumentWithExistingTypesNoThrow, PugiXMLReaderTestFixture) {
    std::string correctDocument = "<?xml version=\"1.0\"?>"
                                   "<ExposedModelUpdate xmlns=\"http://cloudviz.sintef.no/V1/model\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://cloudviz.sintef.no/V1/model/ExposedModelUpdateSchema.xsd\" revision=\"10456\">"
                                   "    <StateSchema xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">"
                                   "        <xsd:schema>"
                                   "            <xsd:element name=\"State\">"
                                   "                 <xsd:complexType>"
                                   "                      <xsd:all>"
                                   "                          <xsd:element name=\"stringKey\" type=\"xsd:string\" />"
                                   "                           <xsd:element name=\"intKey\" type=\"xsd:integer\" />"
                                   "                      </xsd:all>"
                                   "                  </xsd:complexType>"
                                   "            </xsd:element>"
                                   "        </xsd:schema>"
                                   "   </StateSchema>"
                                   "   <State>"
                                   "       <stringKey>new value</stringKey>"
                                   "       <intKey>4</intKey>"
                                   "   </State>"
                                   "   <GuiLayout>"
                                   "   </GuiLayout>"
                                   "</ExposedModelUpdate>";

    model->addElement("stringKey", std::string("value"));
    model->addElement("intKey", 4);

    BOOST_CHECK_NO_THROW(reader.readSchema(correctDocument));
    BOOST_CHECK_NO_THROW(reader.readState(correctDocument));

    BOOST_CHECK_EQUAL("new value", model->getElementValue<std::string>("stringKey"));
    BOOST_CHECK_EQUAL(4, model->getElementValue<int>("intKey"));
}

BOOST_FIXTURE_TEST_CASE(SchemaReadCheckAddedRestriction, PugiXMLReaderTestFixture) {
    std::string correctDocument = "<?xml version=\"1.0\"?>"
                                   "<ExposedModelUpdate xmlns=\"http://cloudviz.sintef.no/V1/model\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://cloudviz.sintef.no/V1/model/ExposedModelUpdateSchema.xsd\" revision=\"10456\">"
                                   "    <StateSchema xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">"
                                   "        <xsd:schema>"
                                   "            <xsd:element name=\"State\">"
                                   "                 <xsd:complexType>"
                                   "                      <xsd:all>"
                                   "                          <xsd:element name=\"stringKey\" type=\"xsd:string\">"
                                   "                           <xsd:simpleType><xsd:restriction base='xsd:string'><xsd:enumeration value=\"something\" /></xsd:restriction></xsd:simpleType></xsd:element>"
                                   "                           <xsd:element name=\"intKey\" type=\"xsd:integer\"><xsd:restriction><xsd:minInclusive value=\"400\" /> <xsd:maxInclusive value=\"600\" /></xsd:restriction></xsd:element>"
                                   "                      </xsd:all>"
                                   "                  </xsd:complexType>"
                                   "            </xsd:element>"
                                   "        </xsd:schema>"
                                   "   </StateSchema>"
                                   "   <State>"
                                   "       <stringKey>something</stringKey>"
                                   "       <intKey>500</intKey>"
                                   "   </State>"
                                   "   <GuiLayout>"
                                   "   </GuiLayout>"
                                   "</ExposedModelUpdate>";

    model->addElement("stringKey", std::string("value"));
    model->addElement("intKey", 4);

   reader.readSchema(correctDocument);
    reader.readState(correctDocument);

    BOOST_CHECK_EQUAL("something", model->getElementValue<std::string>("stringKey"));
    BOOST_CHECK_EQUAL(500, model->getElementValue<int>("intKey"));
}

BOOST_FIXTURE_TEST_CASE(LargeSchemaTest, PugiXMLReaderTestFixture) {
    std::string largeSchema = "<?xml version=\"1.0\"?>"
            "<ExposedModelUpdate xmlns=\"http://cloudviz.sintef.no/V1/model\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://cloudviz.sintef.no/V1/model/ExposedModelUpdateSchema.xsd\" revision=\"10456\">"
            "    <StateSchema xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">"
            "        <xsd:schema>"
            "            <xsd:element name=\"State\">"
            "                <xsd:complexType>"
            "                    <xsd:all>"
            "                        <xsd:element name=\"someBool\" type=\"xsd:bool\" />"
            "                        <xsd:element name=\"someInt\" type=\"xsd:integer\" />"
            "                        <xsd:element name=\"conInt\" type=\"xsd:integer\">"
            "                            <xsd:restriction>"
            "                                <xsd:minInclusive value=\"0\"/>"
            "                                <xsd:maxInclusive value=\"100\"/>"
            "                            </xsd:restriction>"
            "                        </xsd:element>"
            "                        <xsd:element name=\"kindaAdvanced\" type=\"xsd:string\"/>"
            "                        <xsd:element name=\"mesh_2\" type=\"xsd:bool\">"
            "                            <xsd:annotation>"
            "                                <xsd:documentation xml:lang=\"en\">Show mesh 2</xsd:documentation>"
            "                            </xsd:annotation>"
            "                        </xsd:element>"
            "                        <xsd:element name=\"mesh_0\" type=\"xsd:bool\">"
            "                            <xsd:annotation>"
            "                                <xsd:documentation xml:lang=\"en\">Show mesh 0</xsd:documentation>"
            "                            </xsd:annotation>"
            "                        </xsd:element>"
            "                        <xsd:element name=\"boundingbox\" type=\"xsd:string\"/>"
            "                        <xsd:element name=\"alsoKindaAdvanced\" type=\"xsd:string\"/>"
            "                        <xsd:element name=\"timestep\">"
            "                            <xsd:simpleType>"
            "                                <xsd:restriction base=\"xsd:integer\">"
            "                                    <xsd:enumeration value=\"5\"/>"
            "                                    <xsd:enumeration value=\"3\"/>"
            "                                    <xsd:enumeration value=\"28\"/>"
            "                                    <xsd:enumeration value=\"33\"/>"
            "                                    <xsd:enumeration value=\"4\"/>"
            "                                    <xsd:enumeration value=\"23\"/>"
            "                                    <xsd:enumeration value=\"13\"/>"
            "                                    <xsd:enumeration value=\"27\"/>"
            "                                    <xsd:enumeration value=\"14\"/>"
            "                                    <xsd:enumeration value=\"32\"/>"
            "                                    <xsd:enumeration value=\"17\"/>"
            "                                    <xsd:enumeration value=\"12\"/>"
            "                                    <xsd:enumeration value=\"36\"/>"
            "                                    <xsd:enumeration value=\"8\"/>"
            "                                    <xsd:enumeration value=\"26\"/>"
            "                                    <xsd:enumeration value=\"24\"/>"
            "                                    <xsd:enumeration value=\"6\"/>"
            "                                    <xsd:enumeration value=\"18\"/>"
            "                                    <xsd:enumeration value=\"19\"/>"
            "                                    <xsd:enumeration value=\"30\"/>"
            "                                    <xsd:enumeration value=\"7\"/>"
            "                                    <xsd:enumeration value=\"9\"/>"
            "                                    <xsd:enumeration value=\"29\"/>"
            "                                    <xsd:enumeration value=\"10\"/>"
            "                                    <xsd:enumeration value=\"22\"/>"
            "                                    <xsd:enumeration value=\"15\"/>"
            "                                    <xsd:enumeration value=\"35\"/>"
            "                                    <xsd:enumeration value=\"20\"/>"
            "                                    <xsd:enumeration value=\"31\"/>"
            "                                    <xsd:enumeration value=\"25\"/>"
            "                                    <xsd:enumeration value=\"16\"/>"
            "                                    <xsd:enumeration value=\"34\"/>"
            "                                    <xsd:enumeration value=\"2\"/>"
            "                                    <xsd:enumeration value=\"1\"/>"
            "                                    <xsd:enumeration value=\"21\"/>"
            "                                    <xsd:enumeration value=\"11\"/>"
            "                                </xsd:restriction>"
            "                            </xsd:simpleType>"
            "                            <xsd:annotation>"
            "                                <xsd:documentation xml:lang=\"en\">Timestep</xsd:documentation>"
            "                            </xsd:annotation>"
            "                        </xsd:element>"
            "                        <xsd:element name=\"showConfig\" type=\"xsd:bool\">"
            "                            <xsd:annotation>"
            "                                <xsd:documentation xml:lang=\"en\">Configuration</xsd:documentation>"
            "                            </xsd:annotation>"
            "                        </xsd:element>"
            "                        <xsd:element name=\"showAdvanced\" type=\"xsd:bool\">"
            "                            <xsd:annotation>"
            "                                <xsd:documentation xml:lang=\"en\">Enable/show advanced dummy fields</xsd:documentation>"
            "                            </xsd:annotation>"
            "                        </xsd:element>"
            "                        <xsd:element name=\"mesh_1\" type=\"xsd:bool\">"
            "                            <xsd:annotation>"
            "                                <xsd:documentation xml:lang=\"en\">Show mesh 1</xsd:documentation>"
            "                            </xsd:annotation>"
            "                        </xsd:element>"
            "                        <xsd:complexType name=\"viewer\">"
            "                            <xsd:sequence>"
            "                                <xsd:element name=\"width\" type=\"xsd:integer\"/>"
            "                                <xsd:element name=\"height\" type=\"xsd:integer\"/>"
            "                                <xsd:element name=\"projection\">"
            "                                    <xsd:restriction>"
            "                                        <xsd:simpleType>"
            "                                            <xsd:list itemType=\"xsd:float\"/>"
            "                                        </xsd:simpleType>"
            "                                        <xsd:length value=\"16\"/>"
            "                                    </xsd:restriction>"
            "                                </xsd:element>"
            "                                <xsd:element name=\"modelview\">"
            "                                    <xsd:restriction>"
            "                                        <xsd:simpleType>"
            "                                            <xsd:list itemType=\"xsd:float\"/>"
            "                                        </xsd:simpleType>"
            "                                        <xsd:length value=\"16\"/>"
            "                                    </xsd:restriction>"
            "                                </xsd:element>"
            "                                <xsd:element name=\"timestamp\" type=\"xsd:double\"/>"
            "                                <xsd:element name=\"sceneView\" type=\"xsd:string\"/>"
            "                            </xsd:sequence>"
            "                        </xsd:complexType>"
            "                    </xsd:all>"
            "                </xsd:complexType>"
            "            </xsd:element>"
            "        </xsd:schema>"
            "    </StateSchema>"
            "    <State>"
            "        <someBool>0</someBool>"
            "        <conInt>12</conInt>"
            "        <someInt>100</someInt>"
            "        <kindaAdvanced>Some text</kindaAdvanced>"
            "        <mesh_2>0</mesh_2>"
            "        <mesh_0>1</mesh_0>"
            "        <boundingbox>-1.000000 -1 -1 1 1 1 </boundingbox>"
            "        <alsoKindaAdvanced>Some other text</alsoKindaAdvanced>"
            "        <timestep>33</timestep>"
            "        <showConfig>0</showConfig>"
            "        <showAdvanced>0</showAdvanced>"
            "        <mesh_1>0</mesh_1>"
            "        <viewer>"
            "            <width>782</width>"
            "            <height>782</height>"
            "            <projection>1 0 0 0 0 1 0 0 0 0 -1.0202020406723022 -1 0 0 -18.5638427734375 0</projection>"
            "            <modelview>0.7354713678359985 -0.6516221761703491 0.18566226959228516 0 0.2348913997411728 0.502234697341919 0.832217812538147 0 -0.6355375647544861 -0.5684619545936584 0.5224396586418152 0 -65.47185516357422 -58.561859130859375 -436.5816650390625 1</modelview>"
            "            <timestamp>20</timestamp>"
            "            <sceneView>---oooOOOooo---</sceneView>"
            "        </viewer>"
            "    </State>"
            "    <GuiLayout>"
            ""
            "        <HorizontalLayout>"
            "            <TabLayout>"
            "                <Tab key=\"showAdvanced\">"
            "                    <Grid>"
            "                        <Row>"
            "                            <Cell>"
            "                                <Label key=\"timestep\" showValue=\"0\"/>"
            "                            </Cell>"
            "                            <Cell>"
            "                                <Label key=\"timestep\" showValue=\"1\"/>"
            "                            </Cell>"
            "                        </Row>"
            "                        <Row>"
            "                            <Cell>"
            "                                <SpinBox key=\"someInt\" />"
            "                            </Cell>"
            "                        </Row>"
            "                        <Row>"
            "                            <Cell>"
            "                                <TextInput key=\"someInt\" />"
            "                            </Cell>"
            "                        </Row>"
            "                        <Row>"
            "                            <Cell>"
            "                                <TextInput key=\"someInt\" />"
            "                            </Cell>"
            "                        </Row>"
            "                        <Row>"
            "                            <Cell>"
            "                                <SpinBox key=\"conInt\" />"
            "                            </Cell>"
            "                        </Row>"
            "                        <Row>"
            "                            <Cell>"
            "                                <TextInput key=\"conInt\" />"
            "                            </Cell>"
            "                        </Row>"
            "                        <Row>"
            "                            <Cell>"
            "                                <TextInput key=\"conInt\" />"
            "                            </Cell>"
            "                        </Row>"
            ""
            "                        <Row>"
            "                            <Cell>"
            "                                <VerticalSlider key=\"conInt\" />"
            "                            </Cell>"
            "                        </Row>"
            "                        <Row>"
            "                            <Cell>"
            "                                <HorizontalSlider key=\"conInt\" />"
            "                            </Cell>"
            "                        </Row>"
            "                        <Row>"
            "                            <Cell>"
            "                                <Button key=\"someBool\" showValue=\"0\" />"
            "                            </Cell>"
            "                        </Row>"
            "                        <Row>"
            "                            <Cell>"
            "                                <Label key=\"someBool\" showValue=\"1\" />"
            "                            </Cell>"
            "                        </Row>"
            "                        <Row>"
            "                            <Cell>"
            "                                <PopupButton key=\"showConfig\" showValue=\"0\">"
            "                                    <Grid>"
            "                                        <Row>"
            "                                            <Cell>"
            "                                                <Label key=\"timestep\" showValue=\"0\"/>"
            "                                            </Cell>"
            "                                            <Cell>"
            "                                                <Combobox key=\"timestep\" showValue=\"1\"/>"
            "                                            </Cell>"
            "                                            <Cell>"
            "                                                <RadioButtons key=\"timestep\" showValue=\"1\" />"
            "                                            </Cell>"
            "                                        </Row>"
            "                                        <Row>"
            "                                            <Cell/>"
            "                                            <Cell>"
            "                                                <VerticalLayout>"
            "                                                    <Checkbox key=\"mesh_0\" showValue=\"1\"/>"
            "                                                    <Checkbox key=\"mesh_1\" showValue=\"1\"/>"
            "                                                    <Checkbox key=\"mesh_2\" showValue=\"1\"/>"
            "                                                </VerticalLayout>"
            "                                            </Cell>"
            "                                        </Row>"
            "                                        <Row>"
            "                                            <Cell/>"
            "                                            <Cell>"
            "                                                <VerticalExpandingSpace/>"
            "                                            </Cell>"
            "                                        </Row>"
            "                                        <Row>"
            "                                            <Cell>"
            "                                                <Checkbox key=\"showAdvanced\" showValue=\"1\"/>"
            "                                            </Cell>"
            "                                            <Cell/>"
            "                                        </Row>"
            "                                        <Row>"
            "                                            <Cell>"
            "                                                <TextInput key=\"kindaAdvanced\" showValue=\"1\" enabledKey=\"showAdvanced\" enabledKeyInverted=\"0\"/>"
            "                                            </Cell>"
            "                                            <Cell/>"
            "                                        </Row>"
            "                                        <Row>"
            "                                            <Cell>"
            "                                                <TextInput key=\"alsoKindaAdvanced\" showValue=\"1\" visibilityKey=\"showAdvanced\" visibilityKeyInverted=\"0\"/>"
            "                                            </Cell>"
            "                                            <Cell/>"
            "                                        </Row>"
            "                                        <Row>"
            "                                            <Cell/>"
            "                                            <Cell/>"
            "                                        </Row>"
            "                                        <Row>"
            "                                            <Cell/>"
            "                                            <Cell/>"
            "                                        </Row>"
            "                                    </Grid>"
            "                                </PopupButton>"
            "                            </Cell>"
            "                            <Cell/>"
            "                        </Row>"
            "                        <Row>"
            "                            <Cell>"
            "                                <VerticalExpandingSpace/>"
            "                            </Cell>"
            "                            <Cell/>"
            "                        </Row>"
            "                    </Grid>"
            ""
            "                </Tab>"
            "                <Tab key=\"someBool\"></Tab>"
            "            </TabLayout>"
            "            <Grid>"
            "                <Row>"
            "                    <Cell>"
            "                        <Canvas key=\"viewer\" showValue=\"1\" viewerKey=\"viewer\" renderlistKey=\"viewer_renderlist\" boundingboxKey=\"boundingbox\">"
            "                            <scripts>"
            "                                <script className=\"DSRV\">"
            "                                    <parameter name=\"boundingBoxKey\" value=\"boundingbox\"/>"
            "                                    <parameter name=\"key\" value=\"viewer\"/>"
            "                                    <parameter name=\"renderlistKey\" value=\"renderlist\"/>"
            "                                    <parameter name=\"resetViewKey\" value=\"viewer_reset_view\"/>"
            "                                </script>"
            "                            </scripts>"
            "                        </Canvas>"
            "                    </Cell>"
            "                </Row>"
            "            </Grid>"
            ""
            "        </HorizontalLayout>"
            ""
            "    </GuiLayout>"
            "</ExposedModelUpdate>";

    reader.readSchema(largeSchema);
    reader.readState(largeSchema);

    BOOST_CHECK_EQUAL(33, model->getElementValue<int>("timestep"));
    BOOST_CHECK_EQUAL(false, model->getElementValue<bool>("showConfig"));

}

BOOST_AUTO_TEST_SUITE_END()
