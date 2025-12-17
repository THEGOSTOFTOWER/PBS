#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/factory/parser_factory.h"
#include "../src/adapter/idocument_parser.h"
#include "../src/exception/custom_exceptions.h"
#include "../src/textfilters/filter_factory.h"
#include "../src/textfilters/text_filter.h"
#include <memory>
#include <string>
#include <filesystem>

using namespace testing;

class MockParcer: public TIDocumentParser {
public:
    MOCK_METHOD(TDocumentInfo, Parse, (const std::string& filepath), (override));
    MOCK_METHOD(bool, SupportsFormat, (const std::string& format), (const, override));
};

class TParserFactoryTest: public Test {
protected:
    void SetUp() override {
        factory = &TParserFactory::GetInstance();

        mockRegistry.clear();
    }

    void TearDown() override {
        factory = nullptr;
        mockRegistry.clear();
    }

    std::unique_ptr<TIDocumentParser> CreateMockParcer(const std::string& format) {
        auto it = mockRegistry.find(format);
        if (it != mockRegistry.end()) {
            return it->second();
        }
        return nullptr;
    }

    void RegisterMockParcer(const std::string& format,
                            std::function<std::unique_ptr<TIDocumentParser>()> creator) {
        mockRegistry[format] = std::move(creator);
    }

    TParserFactory* factory;
    std::unordered_map<std::string, std::function<std::unique_ptr<TIDocumentParser>()>> mockRegistry;
};

TEST_F(TParserFactoryTest, CreateUnregisteredParcerThrows) {
    EXPECT_THROW({ factory->CreateParser("unsupported"); }, TUnsupportedFormatException);
}

TEST_F(TParserFactoryTest, CreateParserForFileWithExtension) {
    auto& realFactory = TParserFactory::GetInstance();

    auto pdfParcer = realFactory.CreateParserForFile("document.pdf");
    ASSERT_NE(pdfParcer, nullptr);
    EXPECT_TRUE(pdfParcer->SupportsFormat("pdf"));

    auto docxParcer = realFactory.CreateParserForFile("document.docx");
    ASSERT_NE(docxParcer, nullptr);
    EXPECT_TRUE(docxParcer->SupportsFormat("docx"));

    auto txtParcer = realFactory.CreateParserForFile("document.txt");
    ASSERT_NE(txtParcer, nullptr);
    EXPECT_TRUE(txtParcer->SupportsFormat("txt"));
}

TEST_F(TParserFactoryTest, CreateParserForFileWithoutExtension) {
    EXPECT_THROW({ factory->CreateParserForFile("document"); }, TUnsupportedFormatException);
}

TEST_F(TParserFactoryTest, CreateParserForFileWithMultipleDots) {
    auto& realFactory = TParserFactory::GetInstance();

    auto parcer = realFactory.CreateParserForFile("my.document.pdf");
    ASSERT_NE(parcer, nullptr);
    EXPECT_TRUE(parcer->SupportsFormat("pdf"));
}

TEST_F(TParserFactoryTest, CaseInsensitiveFormatSupport) {
    auto& realFactory = TParserFactory::GetInstance();

    EXPECT_NO_THROW(realFactory.CreateParser("PDF"));
    EXPECT_NO_THROW(realFactory.CreateParser("DOCX"));
    EXPECT_NO_THROW(realFactory.CreateParser("TXT"));

    EXPECT_NO_THROW(realFactory.CreateParser("Pdf"));
    EXPECT_NO_THROW(realFactory.CreateParser("Docx"));
    EXPECT_NO_THROW(realFactory.CreateParser("Txt"));
}

class OutputCapture {
    std::streambuf* old_cout;
    std::ostringstream buffer;

public:
    OutputCapture() {
        old_cout = std::cout.rdbuf(buffer.rdbuf());
    }

    ~OutputCapture() {
        std::cout.rdbuf(old_cout);
    }

    std::string getOutput() const {
        return buffer.str();
    }

    void clear() {
        buffer.str("");
    }
};

// Тестовый класс для FilterFactory
class FilterFactoryTest: public ::testing::Test {
protected:
    OutputCapture output;

    void SetUp() override {
    }

    void TearDown() override {
    }

    std::string getCapturedOutput() {
        return output.getOutput();
    }

    void clearOutput() {
        output.clear();
    }
};

TEST_F(FilterFactoryTest, LowerCaseFilterPrintsOutput) {
    auto filter = TFilterFactory::CreateFilter("lowercase");
    ASSERT_NE(filter, nullptr);

    std::string input = "Hello WORLD!";

    std::stringstream captured;
    std::streambuf* old = std::cout.rdbuf(captured.rdbuf());

    filter->Apply(input);

    std::cout.rdbuf(old);

    std::string result = captured.str();

    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    if (!result.empty() && result.back() == '\r') {
        result.pop_back();
    }

    EXPECT_EQ(result, "Lower case: hello world!");
}

TEST_F(FilterFactoryTest, UpperCaseFilterPrintsOutput) {
    auto filter = TFilterFactory::CreateFilter("uppercase");
    ASSERT_NE(filter, nullptr);

    std::string input = "Hello World!";

    std::stringstream captured;
    std::streambuf* old = std::cout.rdbuf(captured.rdbuf());

    filter->Apply(input);

    std::cout.rdbuf(old);

    std::string result = captured.str();

    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    if (!result.empty() && result.back() == '\r') {
        result.pop_back();
    }

    EXPECT_EQ(result, "Upper case: HELLO WORLD!");
}

TEST_F(FilterFactoryTest, NoPunctuationFilterPrintsOutput) {
    auto filter = TFilterFactory::CreateFilter("nopunctuation");
    ASSERT_NE(filter, nullptr);

    std::string input = "Hello, World! How are you?";

    std::stringstream captured;
    std::streambuf* old = std::cout.rdbuf(captured.rdbuf());

    filter->Apply(input);

    std::cout.rdbuf(old);

    std::string result = captured.str();

    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    if (!result.empty() && result.back() == '\r') {
        result.pop_back();
    }

    EXPECT_EQ(result, "No punctuation: Hello World How are you");
}

TEST_F(FilterFactoryTest, WordCountFilterPrintsWordCount) {
    auto filter = TFilterFactory::CreateFilter("wordcount");
    ASSERT_NE(filter, nullptr);

    std::string input = "Hello World! This is a test.";

    std::stringstream captured;
    std::streambuf* old = std::cout.rdbuf(captured.rdbuf());

    filter->Apply(input);

    std::cout.rdbuf(old);

    std::string result = captured.str();

    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    if (!result.empty() && result.back() == '\r') {
        result.pop_back();
    }

    EXPECT_EQ(result, "[WordCount] Words found: 6");
}

TEST_F(FilterFactoryTest, EmptyInputHandling) {
    auto filter = TFilterFactory::CreateFilter("lowercase");
    ASSERT_NE(filter, nullptr);

    std::string input = "";

    std::stringstream captured;
    std::streambuf* old = std::cout.rdbuf(captured.rdbuf());

    filter->Apply(input);

    std::cout.rdbuf(old);

    std::string result = captured.str();

    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    if (!result.empty() && result.back() == '\r') {
        result.pop_back();
    }

    EXPECT_EQ(result, "Lower case: ");
}

TEST_F(FilterFactoryTest, SpecialCharactersHandling) {
    auto filter = TFilterFactory::CreateFilter("lowercase");
    ASSERT_NE(filter, nullptr);

    std::string input = "TEST: 123 @#$%";

    std::stringstream captured;
    std::streambuf* old = std::cout.rdbuf(captured.rdbuf());

    filter->Apply(input);

    std::cout.rdbuf(old);

    std::string result = captured.str();

    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    if (!result.empty() && result.back() == '\r') {
        result.pop_back();
    }

    EXPECT_EQ(result, "Lower case: test: 123 @#$%");
}

TEST_F(FilterFactoryTest, MultiLineTextHandling) {
    auto filter = TFilterFactory::CreateFilter("uppercase");
    ASSERT_NE(filter, nullptr);

    std::string input = "First line\nSecond line\nThird line";

    std::stringstream captured;
    std::streambuf* old = std::cout.rdbuf(captured.rdbuf());

    filter->Apply(input);

    std::cout.rdbuf(old);

    std::string result = captured.str();

    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }

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