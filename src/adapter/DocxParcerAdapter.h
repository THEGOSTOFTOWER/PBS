#pragma once

#include "IDocumentParcer.h"
#include <zip.h>
#include <pugixml.hpp>

class DocxParcerAdapter: public IDocumentParcer {
public:
    bool SupportsFormat(const std::string& format) const override {
        return format == "docx";
    }

    DocumentInfo Parse(const std::string& file_path) override {
        try {
            int err = 0;
            zip* archive = zip_open(file_path.c_str(), 0, &err);
            if (!archive) {
                throw DocumentProcessingException("Failed to open DOCX file");
            }

            const char* xml_path = "word/document.xml";
            struct zip_stat stat;
            if (zip_stat(archive, xml_path, 0, &stat) != 0) {
                zip_close(archive);
                throw DocumentProcessingException("Failed to find word/document.xml in DOCX");
            }

            zip_file* xml_file = zip_fopen(archive, xml_path, 0);
            if (!xml_file) {
                zip_close(archive);
                throw DocumentProcessingException("Failed to open word/document.xml");
            }

            std::string xml;
            char buffer[4096];
            zip_int64_t bytes = 0;
            while ((bytes = zip_fread(xml_file, buffer, sizeof(buffer))) > 0) {
                xml.append(buffer, bytes);
            }

            zip_fclose(xml_file);
            zip_close(archive);

            pugi::xml_document doc;
            if (!doc.load_string(xml.c_str())) {
                throw DocumentProcessingException("Failed to parse DOCX XML");
            }

            DocumentInfo info;

            for (auto node : doc.select_nodes("//w:t")) {
                info.text += node.node().text().get();
                info.text += '\n';
            }

            info.metadata = {{"format", "docx"}};

            return info;
        } catch (const DocumentProcessingException& e) {
            throw ParsingException(file_path, std::string(e.what()));
        }
    }
};