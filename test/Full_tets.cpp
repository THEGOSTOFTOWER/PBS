#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/parsers/parser_factory.h"
#include "../src/parsers/document_parser.h"
#include "../src/exception/custom_exceptions.h"
#include "../src/textfilters/filter_factory.h"
#include "../src/textfilters/text_filter.h"
#include "../src/legacy_parsers/legacy_pdf_parser.h"
#include "../src/parsers/pdf_parser_adapter.h"
#include "../src/parsers/docx_parser_adapter.h"
#include "../src/parsers/text_parser_adapter.h"

#include <memory>
#include <string>
#include <filesystem>

using namespace testing;

class TParserFactoryTest: public Test {
protected:
    void SetUp() override {
        factory = &TParserFactory::GetInstance();
    }

    void TearDown() override {
        factory = nullptr;
    }

    TParserFactory* factory;
};

TEST_F(TParserFactoryTest, CreateUnregisteredParcerThrows) {
    EXPECT_THROW({ factory->CreateParser("unsupported"); }, TUnsupportedFormatException);
}

TEST_F(TParserFactoryTest, CreateParserForFileWithExtension) {
    auto pdfParcer = factory->CreateParserForFile("document.pdf");
    ASSERT_NE(pdfParcer, nullptr);
    EXPECT_TRUE(pdfParcer->SupportsFormat("pdf"));

    auto docxParcer = factory->CreateParserForFile("document.docx");
    ASSERT_NE(docxParcer, nullptr);
    EXPECT_TRUE(docxParcer->SupportsFormat("docx"));

    auto txtParcer = factory->CreateParserForFile("document.txt");
    ASSERT_NE(txtParcer, nullptr);
    EXPECT_TRUE(txtParcer->SupportsFormat("txt"));
}

TEST_F(TParserFactoryTest, CreateParserForFileWithoutExtension) {
    EXPECT_THROW({ factory->CreateParserForFile("document"); }, TUnsupportedFormatException);
}

TEST_F(TParserFactoryTest, CreateParserForFileWithMultipleDots) {
    auto parcer = factory->CreateParserForFile("my.document.pdf");
    ASSERT_NE(parcer, nullptr);
    EXPECT_TRUE(parcer->SupportsFormat("pdf"));
}

TEST_F(TParserFactoryTest, CaseInsensitiveFormatSupport) {
    EXPECT_NO_THROW(factory->CreateParser("PDF"));
    EXPECT_NO_THROW(factory->CreateParser("DOCX"));
    EXPECT_NO_THROW(factory->CreateParser("TXT"));

    EXPECT_NO_THROW(factory->CreateParser("Pdf"));
    EXPECT_NO_THROW(factory->CreateParser("Docx"));
    EXPECT_NO_THROW(factory->CreateParser("Txt"));
}

class FilterFactoryTest: public ::testing::Test {
};

TEST_F(FilterFactoryTest, LowerCaseFilterPrintsOutput) {
    auto filter = TFilterFactory::CreateFilter("lowercase");
    ASSERT_NE(filter, nullptr);

    std::string input = "Hello WORLD!";

    std::string result = filter->Apply(input);

    EXPECT_EQ(result, "Lower case: hello world!");
}

TEST_F(FilterFactoryTest, UpperCaseFilterPrintsOutput) {
    auto filter = TFilterFactory::CreateFilter("uppercase");
    ASSERT_NE(filter, nullptr);

    std::string input = "Hello World!";

    std::string result = filter->Apply(input);

    EXPECT_EQ(result, "Upper case: HELLO WORLD!");
}

TEST_F(FilterFactoryTest, NoPunctuationFilterPrintsOutput) {
    auto filter = TFilterFactory::CreateFilter("nopunctuation");
    ASSERT_NE(filter, nullptr);

    std::string input = "Hello, World! How are you?";

    std::string result = filter->Apply(input);

    EXPECT_EQ(result, "No punctuation: Hello World How are you");
}

TEST_F(FilterFactoryTest, WordCountFilterPrintsWordCount) {
    auto filter = TFilterFactory::CreateFilter("wordcount");
    ASSERT_NE(filter, nullptr);

    std::string input = "Hello World! This is a test.";

    std::string result = filter->Apply(input);

    EXPECT_EQ(result, "[WordCount] Words found: 6");
}

TEST_F(FilterFactoryTest, EmptyInputHandling) {
    auto filter = TFilterFactory::CreateFilter("lowercase");
    ASSERT_NE(filter, nullptr);

    std::string input = "";

    std::string result = filter->Apply(input);

    EXPECT_EQ(result, "Lower case: ");
}

TEST_F(FilterFactoryTest, SpecialCharactersHandling) {
    auto filter = TFilterFactory::CreateFilter("lowercase");
    ASSERT_NE(filter, nullptr);

    std::string input = "TEST: 123 @#$%";

    std::string result = filter->Apply(input);

    EXPECT_EQ(result, "Lower case: test: 123 @#$%");
}

TEST_F(FilterFactoryTest, MultiLineTextHandling) {
    auto filter = TFilterFactory::CreateFilter("uppercase");
    ASSERT_NE(filter, nullptr);

    std::string input = "First line\nSecond line\nThird line";

    std::string result = filter->Apply(input);

    EXPECT_EQ(result, "Upper case: FIRST LINE\nSECOND LINE\nTHIRD LINE");
}

TEST_F(FilterFactoryTest, UnknownFilterThrowsException) {
    EXPECT_THROW({ TFilterFactory::CreateFilter("unknown"); }, FilterException);
}

TEST_F(FilterFactoryTest, CreateAllFilters) {
    EXPECT_NO_THROW(TFilterFactory::CreateFilter("lowercase"));
    EXPECT_NO_THROW(TFilterFactory::CreateFilter("uppercase"));
    EXPECT_NO_THROW(TFilterFactory::CreateFilter("nopunctuation"));
    EXPECT_NO_THROW(TFilterFactory::CreateFilter("wordcount"));
}

TEST_F(FilterFactoryTest, GetSupportedFormatsReturnsCorrectString) {
    std::string formats = TFilterFactory::GetSupportedFormats();

    EXPECT_NE(formats.find("lowercase"), std::string::npos);
    EXPECT_NE(formats.find("uppercase"), std::string::npos);
    EXPECT_NE(formats.find("nopunctuation"), std::string::npos);
    EXPECT_NE(formats.find("wordcount"), std::string::npos);
}

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::StrictMock;
using ::testing::Throw;

class MockLegacyPdfParser: public TLegacyPdfParser {
public:
    MOCK_METHOD(TDocumentInfo, Parse, (const std::string& file_path), (override));
};

class MockLegacyTextParser: public TLegacyTextParser {
public:
    MOCK_METHOD(TDocumentInfo, Parse, (const std::string& file_path), (override));
};

class MockLegacyDocxParser: public TLegacyDocxParser {
public:
    MOCK_METHOD(TDocumentInfo, Parse, (const std::string& file_path), (override));
};

class TPdfParserAdapterTest: public ::testing::Test {
protected:
    void SetUp() override {
        mockParser = std::make_unique<NiceMock<MockLegacyPdfParser>>();
    }

    std::unique_ptr<MockLegacyPdfParser> mockParser;
};

TEST_F(TPdfParserAdapterTest, DelegatesToLegacyParser) {
    const std::string testFilePath = "document.pdf";
    TDocumentInfo expectedInfo;
    expectedInfo.text = "PDF content from legacy parser";
    expectedInfo.metadata = {{"pages", "5"}, {"author", "Test Author"}};

    EXPECT_CALL(*mockParser, Parse(testFilePath))
        .WillOnce(Return(expectedInfo));

    TPdfParserAdapter adapter(std::move(mockParser));

    TDocumentInfo result = adapter.Parse(testFilePath);

    EXPECT_EQ(result.text, expectedInfo.text);
    EXPECT_EQ(result.metadata, expectedInfo.metadata);
}

TEST_F(TPdfParserAdapterTest, SupportsFormatCorrectly) {
    TPdfParserAdapter adapter(std::make_unique<MockLegacyPdfParser>());

    EXPECT_TRUE(adapter.SupportsFormat("pdf"));
    EXPECT_FALSE(adapter.SupportsFormat(".pdf"));
    EXPECT_FALSE(adapter.SupportsFormat("PDF"));

    EXPECT_FALSE(adapter.SupportsFormat("docx"));
    EXPECT_FALSE(adapter.SupportsFormat("txt"));
    EXPECT_FALSE(adapter.SupportsFormat(""));
}

TEST_F(TPdfParserAdapterTest, HandlesEmptyFilePath) {
    const std::string emptyPath = "";
    TDocumentInfo expectedInfo;
    expectedInfo.text = "";
    expectedInfo.metadata = {{"error", "empty path"}};

    EXPECT_CALL(*mockParser, Parse(emptyPath))
        .WillOnce(Return(expectedInfo));

    TPdfParserAdapter adapter(std::move(mockParser));

    TDocumentInfo result = adapter.Parse(emptyPath);
    EXPECT_TRUE(result.text.empty());
    EXPECT_EQ(result.metadata["error"], "empty path");
}

class TTextParserAdapterTest: public ::testing::Test {
protected:
    void SetUp() override {
        mockParser = std::make_unique<NiceMock<MockLegacyTextParser>>();
    }

    std::unique_ptr<MockLegacyTextParser> mockParser;
};

TEST_F(TTextParserAdapterTest, DelegatesToLegacyParser) {
    const std::string testFilePath = "document.txt";
    TDocumentInfo expectedInfo;
    expectedInfo.text = "Text content from legacy parser";
    expectedInfo.metadata = {{"lines", "10"}, {"encoding", "UTF-8"}};

    EXPECT_CALL(*mockParser, Parse(testFilePath))
        .WillOnce(Return(expectedInfo));

    TTextParserAdapter adapter(std::move(mockParser));

    TDocumentInfo result = adapter.Parse(testFilePath);

    EXPECT_EQ(result.text, expectedInfo.text);
    EXPECT_EQ(result.metadata, expectedInfo.metadata);
}

TEST_F(TTextParserAdapterTest, SupportsFormatCorrectly) {
    TTextParserAdapter adapter(std::make_unique<MockLegacyTextParser>());

    EXPECT_TRUE(adapter.SupportsFormat("txt"));
    EXPECT_FALSE(adapter.SupportsFormat(".txt"));
    EXPECT_FALSE(adapter.SupportsFormat("TXT"));
    EXPECT_FALSE(adapter.SupportsFormat("text"));

    EXPECT_FALSE(adapter.SupportsFormat("pdf"));
    EXPECT_FALSE(adapter.SupportsFormat("docx"));
    EXPECT_FALSE(adapter.SupportsFormat(""));
}

TEST_F(TTextParserAdapterTest, HandlesEmptyFilePath) {
    const std::string emptyPath = "";
    TDocumentInfo expectedInfo;
    expectedInfo.text = "";
    expectedInfo.metadata = {{"error", "empty path"}};

    EXPECT_CALL(*mockParser, Parse(emptyPath))
        .WillOnce(Return(expectedInfo));

    TTextParserAdapter adapter(std::move(mockParser));

    TDocumentInfo result = adapter.Parse(emptyPath);
    EXPECT_TRUE(result.text.empty());
    EXPECT_EQ(result.metadata["error"], "empty path");
}

TEST_F(TTextParserAdapterTest, PropagatesExceptionsFromLegacyParser) {
    const std::string testFilePath = "corrupted.txt";
    const std::string errorMessage = "Failed to parse text file";

    EXPECT_CALL(*mockParser, Parse(testFilePath))
        .WillOnce(Throw(std::runtime_error(errorMessage)));

    TTextParserAdapter adapter(std::move(mockParser));

    EXPECT_THROW({ adapter.Parse(testFilePath); }, std::runtime_error);
}

class TDocxParserAdapterTest: public ::testing::Test {
protected:
    void SetUp() override {
        mockParser = std::make_unique<NiceMock<MockLegacyDocxParser>>();
    }

    std::unique_ptr<MockLegacyDocxParser> mockParser;
};

TEST_F(TDocxParserAdapterTest, DelegatesToLegacyParser) {
    const std::string testFilePath = "document.docx";
    TDocumentInfo expectedInfo;
    expectedInfo.text = "DOCX content from legacy parser";
    expectedInfo.metadata = {{"pages", "3"}, {"author", "Test Author"}, {"format", "DOCX"}};

    EXPECT_CALL(*mockParser, Parse(testFilePath))
        .WillOnce(Return(expectedInfo));

    TDocxParserAdapter adapter(std::move(mockParser));

    TDocumentInfo result = adapter.Parse(testFilePath);

    EXPECT_EQ(result.text, expectedInfo.text);
    EXPECT_EQ(result.metadata, expectedInfo.metadata);
}

TEST_F(TDocxParserAdapterTest, SupportsFormatCorrectly) {
    TDocxParserAdapter adapter(std::make_unique<MockLegacyDocxParser>());

    EXPECT_TRUE(adapter.SupportsFormat("docx"));
    EXPECT_FALSE(adapter.SupportsFormat(".docx"));
    EXPECT_FALSE(adapter.SupportsFormat("DOCX"));
    EXPECT_FALSE(adapter.SupportsFormat("doc"));

    EXPECT_FALSE(adapter.SupportsFormat("pdf"));
    EXPECT_FALSE(adapter.SupportsFormat("txt"));
    EXPECT_FALSE(adapter.SupportsFormat(""));
}

TEST_F(TDocxParserAdapterTest, HandlesEmptyFilePath) {
    const std::string emptyPath = "";
    TDocumentInfo expectedInfo;
    expectedInfo.text = "";
    expectedInfo.metadata = {{"error", "empty path"}};

    EXPECT_CALL(*mockParser, Parse(emptyPath))
        .WillOnce(Return(expectedInfo));

    TDocxParserAdapter adapter(std::move(mockParser));

    TDocumentInfo result = adapter.Parse(emptyPath);
    EXPECT_TRUE(result.text.empty());
    EXPECT_EQ(result.metadata["error"], "empty path");
}

TEST_F(TDocxParserAdapterTest, PropagatesExceptionsFromLegacyParser) {
    const std::string testFilePath = "corrupted.docx";
    const std::string errorMessage = "Failed to parse DOCX file";

    EXPECT_CALL(*mockParser, Parse(testFilePath))
        .WillOnce(Throw(std::runtime_error(errorMessage)));

    TDocxParserAdapter adapter(std::move(mockParser));

    EXPECT_THROW({ adapter.Parse(testFilePath); }, std::runtime_error);
}

TEST_F(TDocxParserAdapterTest, DefaultConstructorWorks) {
    TDocxParserAdapter adapter;

    EXPECT_TRUE(adapter.SupportsFormat("docx"));

    EXPECT_THROW({ adapter.Parse("non_existent.docx"); }, std::exception);
}