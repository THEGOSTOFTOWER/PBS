#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/factory/ParcerFactory.h"
#include "../src/adapter/IDocumentParcer.h"
#include "../src/exception/CustomException.h"
#include "../src/textfilters/TextFilterFactory.h"
#include "../src/textfilters/TextFilter.h"
#include <memory>
#include <string>
#include <filesystem>

using namespace testing;

class MockParcer: public IDocumentParcer {
public:
    MOCK_METHOD(DocumentInfo, Parse, (const std::string& filepath), (override));
    MOCK_METHOD(bool, SupportsFormat, (const std::string& format), (const, override));
};

class ParcerFactoryTest: public Test {
protected:
    void SetUp() override {
        factory = &ParcerFactory::GetInstance();

        mockRegistry.clear();
    }

    void TearDown() override {
        factory = nullptr;
        mockRegistry.clear();
    }

    std::unique_ptr<IDocumentParcer> CreateMockParcer(const std::string& format) {
        auto it = mockRegistry.find(format);
        if (it != mockRegistry.end()) {
            return it->second();
        }
        return nullptr;
    }

    void RegisterMockParcer(const std::string& format,
                            std::function<std::unique_ptr<IDocumentParcer>()> creator) {
        mockRegistry[format] = std::move(creator);
    }

    ParcerFactory* factory;
    std::unordered_map<std::string, std::function<std::unique_ptr<IDocumentParcer>()>> mockRegistry;
};

TEST_F(ParcerFactoryTest, CreateUnregisteredParcerThrows) {
    EXPECT_THROW({ factory->CreateParcer("unsupported"); }, UnsupportedFormatException);
}

TEST_F(ParcerFactoryTest, CreateParcerForFileWithExtension) {
    auto& realFactory = ParcerFactory::GetInstance();

    auto pdfParcer = realFactory.CreateParcerForFile("document.pdf");
    ASSERT_NE(pdfParcer, nullptr);
    EXPECT_TRUE(pdfParcer->SupportsFormat("pdf"));

    auto docxParcer = realFactory.CreateParcerForFile("document.docx");
    ASSERT_NE(docxParcer, nullptr);
    EXPECT_TRUE(docxParcer->SupportsFormat("docx"));

    auto txtParcer = realFactory.CreateParcerForFile("document.txt");
    ASSERT_NE(txtParcer, nullptr);
    EXPECT_TRUE(txtParcer->SupportsFormat("txt"));
}

TEST_F(ParcerFactoryTest, CreateParcerForFileWithoutExtension) {
    EXPECT_THROW({ factory->CreateParcerForFile("document"); }, UnsupportedFormatException);
}

TEST_F(ParcerFactoryTest, CreateParcerForFileWithMultipleDots) {
    auto& realFactory = ParcerFactory::GetInstance();

    auto parcer = realFactory.CreateParcerForFile("my.document.pdf");
    ASSERT_NE(parcer, nullptr);
    EXPECT_TRUE(parcer->SupportsFormat("pdf"));
}

TEST_F(ParcerFactoryTest, CaseInsensitiveFormatSupport) {
    auto& realFactory = ParcerFactory::GetInstance();

    EXPECT_NO_THROW(realFactory.CreateParcer("PDF"));
    EXPECT_NO_THROW(realFactory.CreateParcer("DOCX"));
    EXPECT_NO_THROW(realFactory.CreateParcer("TXT"));

    EXPECT_NO_THROW(realFactory.CreateParcer("Pdf"));
    EXPECT_NO_THROW(realFactory.CreateParcer("Docx"));
    EXPECT_NO_THROW(realFactory.CreateParcer("Txt"));
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
    auto filter = FilterFactory::CreateFilter("lowercase");
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
    auto filter = FilterFactory::CreateFilter("uppercase");
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
    auto filter = FilterFactory::CreateFilter("nopunctuation");
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
    auto filter = FilterFactory::CreateFilter("wordcount");
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

    EXPECT_EQ(result, "[WordCountFilter] Words found: 6");
}

TEST_F(FilterFactoryTest, EmptyInputHandling) {
    auto filter = FilterFactory::CreateFilter("lowercase");
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
    auto filter = FilterFactory::CreateFilter("lowercase");
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
    auto filter = FilterFactory::CreateFilter("uppercase");
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
    EXPECT_THROW({ FilterFactory::CreateFilter("unknown"); }, FilterException);
}

TEST_F(FilterFactoryTest, CreateAllFilters) {
    EXPECT_NO_THROW(FilterFactory::CreateFilter("lowercase"));
    EXPECT_NO_THROW(FilterFactory::CreateFilter("uppercase"));
    EXPECT_NO_THROW(FilterFactory::CreateFilter("nopunctuation"));
    EXPECT_NO_THROW(FilterFactory::CreateFilter("wordcount"));
}

TEST_F(FilterFactoryTest, GetSupportedFormatsReturnsCorrectString) {
    std::string formats = FilterFactory::GetSupportedFormats();

    EXPECT_NE(formats.find("lowercase"), std::string::npos);
    EXPECT_NE(formats.find("uppercase"), std::string::npos);
    EXPECT_NE(formats.find("nopunctuation"), std::string::npos);
    EXPECT_NE(formats.find("wordcount"), std::string::npos);
}