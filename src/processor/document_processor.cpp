#include "document_processor.h"

TDocumentProcessor::TDocumentProcessor(TParserFactory& factory)
    : factory_(factory) {
}

TDocumentInfo TDocumentProcessor::Process(const std::string& path,
                                          const std::vector<std::string>& filters,
                                          ProgressCallback progress) {
    try {
        progress(10);
        auto parser = factory_.CreateParserForFile(path);
        progress(40);
        TDocumentInfo info = parser->Parse(path);
        progress(70);
        if (filters.empty()) {
            std::cout << info.text << std::endl;
        } else {
            for (const auto& filter : filters) {
                try {
                    TFilterFactory::CreateFilter(filter)->Apply(info.text);
                } catch (FilterException& e) {
                    std::cout << "Supported Filter formats: " + TFilterFactory::GetSupportedFormats() << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << e.what() << '\n';
                }
            }
        }
        progress(100);
        return info;
    } catch (TUnsupportedFormatException& e) {
        std::cout << e.what() << '\n';
        std::cout << "Supported formats: ";
        for (auto format : factory_.GetSupportedFormats()) {
            std::cout << format << ' ';
        }
        std::cout << "\n";
        throw std::runtime_error(e.what());
    } catch (TParsingException& e) {
        std::cout << e.what() << '\n';
        throw std::runtime_error(e.what());
    } catch (TDocumentProcessingException& e) {
        std::cout << e.what() << '\n';
        throw std::runtime_error(e.what());
    } catch (std::exception& e) {
        throw std::runtime_error(e.what());
    }
}