#ifndef CPR_PARAMETERS_H
#define CPR_PARAMETERS_H

#include "cpr/export.h"

/**
 * If we build cpr as C++20 module, we use 'import std;'.
 * So skip all other imports and declare them in 'cpr.cxx'.
 **/
#ifndef CPR_IMPORT_STD
#include <initializer_list>
#include <map>
#include <utility>
#endif

#include "cpr/curl_container.h"

namespace cpr {

EXPORT_CPR class Parameters : public CurlContainer<Parameter> {
  public:
    Parameters() = default;
    Parameters(const std::initializer_list<Parameter>& parameters) : CurlContainer<Parameter>(parameters) {}

    explicit Parameters(const std::map<std::string, std::string>& parameters) {
        containerList_.reserve(parameters.size());

        for (const auto& [key, value] : parameters) {
            this->Add(Parameter{key, value});
        }
    }

    explicit Parameters(std::map<std::string, std::string>&& parameters) {
        auto owned_parameters = std::move(parameters);
        containerList_.reserve(owned_parameters.size());

        for (auto& [key, value] : owned_parameters) {
            this->Add(Parameter{key, std::move(value)});
        }
    }
};

} // namespace cpr

#endif
