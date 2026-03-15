#include "graph/read/read_dynamic_info.hpp"

#include <fstream>
#include <assert.h>

#include "graph/graph.hpp"
#include "pass/log_func.h"

namespace rdi {

enum KeyWord {
    kInvalid = -1,
    kFunc = 0,
    kBasicBlock
};

static DynamicInfoReadError ReadModuleDynamicInfo(graph::Cluster *module, std::ifstream &in);

static KeyWord GetKeyWordEnum(const std::string &key_word);

DynamicInfoReadError ReadDynamicInfo(graph::Cluster *module) {
    if (module == NULL) {
        return DynamicInfoReadError::kInvalidArgument;
    }

    std::ifstream in(kDynamicInfoFileName);
    if (!in.is_open()) {
        return DynamicInfoReadError::kCantOpenFile;
    }

    return ReadModuleDynamicInfo(module, in);
}

static DynamicInfoReadError ReadModuleDynamicInfo(graph::Cluster *module, std::ifstream &in) {
    assert(module);

    DynamicInfoReadError error = DynamicInfoReadError::kDone;
    size_t func_idx = 0;
    graph::Cluster *func = nullptr;
    size_t bb_idx = 0;
    graph::Cluster *bb = nullptr;
    while (!in.eof()) {
        std::string key_word_str;
        in >> key_word_str;
        KeyWord key_word = GetKeyWordEnum(key_word_str);

        switch (key_word) {
            case KeyWord::kFunc : {
                in >> func_idx;
                func = dynamic_cast<graph::Cluster*>(module->GetChild(func_idx));
                if (func == nullptr) {
                    return DynamicInfoReadError::InvalidDynamicInfo;
                }
                func->IncrementEntrance();
                break;
            };

            case KeyWord::kBasicBlock : {
                if (func == nullptr) {
                    return DynamicInfoReadError::InvalidDynamicInfo;
                }
                in >> bb_idx;
                bb = dynamic_cast<graph::Cluster*>(func->GetChild(bb_idx));
                if (bb == nullptr) {
                    return DynamicInfoReadError::InvalidDynamicInfo;
                }
                bb->IncrementEntrance();
                break;
            };

            case KeyWord::kInvalid : {
                return DynamicInfoReadError::kInvalidKeyWord;
            };
        };
    }

    return error;
}

static KeyWord GetKeyWordEnum(const std::string &key_word) {
    if (key_word == kFuncEnterKeyWord) {
        return KeyWord::kFunc;
    }
    if (key_word == kBBEnterKeyWord) {
        return KeyWord::kBasicBlock;
    }
    return KeyWord::kInvalid;
}

}; // namespace rdi
