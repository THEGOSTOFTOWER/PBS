#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/parsers/parser_factory.h"
#include "../src/parsers/idocument_parser.h"
#include "../src/exception/custom_exceptions.h"
#include "../src/textfilters/filter_factory.h"
#include "../src/textfilters/text_filter.h"
#include "../src/legacy_parsers/legacy_pdf_parser.h"
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

    void TearDown() override {
        mockParser.reset();
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

    void TearDown() override {
        mockParser.reset();
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

    void TearDown() override {
        mockParser.reset();
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