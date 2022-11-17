// Copyright 2022 The Tint Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "src/tint/ir/debug.h"

#include <sstream>
#include <unordered_map>
#include <unordered_set>

#include "src/tint/ir/block.h"
#include "src/tint/ir/if.h"
#include "src/tint/ir/loop.h"
#include "src/tint/ir/switch.h"
#include "src/tint/ir/terminator.h"
#include "src/tint/program.h"

namespace tint::ir {
namespace {

class ScopedStopNode {
  public:
    ScopedStopNode(std::unordered_set<const FlowNode*>* stop_nodes, const FlowNode* node)
        : stop_nodes_(stop_nodes), node_(node) {
        stop_nodes_->insert(node_);
    }

    ~ScopedStopNode() { stop_nodes_->erase(node_); }

  private:
    std::unordered_set<const FlowNode*>* stop_nodes_;
    const FlowNode* node_;
};

class ScopedIndent {
  public:
    explicit ScopedIndent(uint32_t* indent) : indent_(indent) { (*indent_) += 2; }

    ~ScopedIndent() { (*indent_) -= 2; }

  private:
    uint32_t* indent_;
};

}  // namespace

// static
std::string Debug::AsDotGraph(const Module* mod) {
    size_t node_count = 0;

    std::unordered_set<const FlowNode*> visited;
    std::unordered_set<const FlowNode*> merge_nodes;
    std::unordered_map<const FlowNode*, std::string> node_to_name;
    std::stringstream out;

    auto name_for = [&](const FlowNode* node) -> std::string {
        if (node_to_name.count(node) > 0) {
            return node_to_name[node];
        }

        std::string name = "node_" + std::to_string(node_count);
        node_count += 1;

        node_to_name[node] = name;
        return name;
    };

    std::function<void(const FlowNode*)> Graph = [&](const FlowNode* node) {
        if (visited.count(node) > 0) {
            return;
        }
        visited.insert(node);

        tint::Switch(
            node,
            [&](const ir::Block* b) {
                if (node_to_name.count(b) == 0) {
                    out << name_for(b) << R"( [label="block"])" << std::endl;
                }
                out << name_for(b) << " -> " << name_for(b->branch_target);

                // Dashed lines to merge blocks
                if (merge_nodes.count(b->branch_target) != 0) {
                    out << " [style=dashed]";
                }

                out << std::endl;
                Graph(b->branch_target);
            },
            [&](const ir::Switch* s) {
                out << name_for(s) << R"( [label="switch"])" << std::endl;
                out << name_for(s->merge_target) << R"( [label="switch merge"])" << std::endl;
                merge_nodes.insert(s->merge_target);

                size_t i = 0;
                for (const auto& c : s->cases) {
                    out << name_for(c.start_target)
                        << R"( [label="case )" + std::to_string(i++) + R"("])" << std::endl;
                }
                out << name_for(s) << " -> {";
                for (const auto& c : s->cases) {
                    if (&c != &(s->cases[0])) {
                        out << ", ";
                    }
                    out << name_for(c.start_target);
                }
                out << "}" << std::endl;

                for (const auto& c : s->cases) {
                    Graph(c.start_target);
                }
                Graph(s->merge_target);
            },
            [&](const ir::If* i) {
                out << name_for(i) << R"( [label="if"])" << std::endl;
                out << name_for(i->true_target) << R"( [label="true"])" << std::endl;
                out << name_for(i->false_target) << R"( [label="false"])" << std::endl;
                out << name_for(i->merge_target) << R"( [label="if merge"])" << std::endl;
                merge_nodes.insert(i->merge_target);

                out << name_for(i) << " -> {";
                out << name_for(i->true_target) << ", " << name_for(i->false_target);
                out << "}" << std::endl;

                // Subgraph if true/false branches so they draw on the same line
                out << "subgraph sub_" << name_for(i) << " {" << std::endl;
                out << R"(rank="same")" << std::endl;
                out << name_for(i->true_target) << std::endl;
                out << name_for(i->false_target) << std::endl;
                out << "}" << std::endl;

                Graph(i->true_target);
                Graph(i->false_target);
                Graph(i->merge_target);
            },
            [&](const ir::Loop* l) {
                out << name_for(l) << R"( [label="loop"])" << std::endl;
                out << name_for(l->start_target) << R"( [label="start"])" << std::endl;
                out << name_for(l->continuing_target) << R"( [label="continuing"])" << std::endl;
                out << name_for(l->merge_target) << R"( [label="loop merge"])" << std::endl;
                merge_nodes.insert(l->merge_target);

                // Subgraph the continuing and merge so they get drawn on the same line
                out << "subgraph sub_" << name_for(l) << " {" << std::endl;
                out << R"(rank="same")" << std::endl;
                out << name_for(l->continuing_target) << std::endl;
                out << name_for(l->merge_target) << std::endl;
                out << "}" << std::endl;

                out << name_for(l) << " -> " << name_for(l->start_target) << std::endl;

                Graph(l->start_target);
                Graph(l->continuing_target);
                Graph(l->merge_target);
            },
            [&](const ir::Terminator*) {
                // Already done
            });
    };

    out << "digraph G {" << std::endl;
    for (const auto* func : mod->functions) {
        // Cluster each function to label and draw a box around it.
        out << "subgraph cluster_" << name_for(func) << " {" << std::endl;
        out << R"(label=")" << mod->program->Symbols().NameFor(func->source->symbol) << R"(")"
            << std::endl;
        out << name_for(func->start_target) << R"( [label="start"])" << std::endl;
        out << name_for(func->end_target) << R"( [label="end"])" << std::endl;
        Graph(func->start_target);
        out << "}" << std::endl;
    }
    out << "}";
    return out.str();
}

// static
std::string Debug::AsString(const Module* mod) {
    std::stringstream out;

    std::unordered_set<const FlowNode*> visited;
    std::unordered_set<const FlowNode*> stop_nodes;
    uint32_t indent_size = 0;

    std::function<std::ostream&(void)> indent = [&]() -> std::ostream& {
        for (uint32_t i = 0; i < indent_size; i++) {
            out << " ";
        }
        return out;
    };

    std::function<void(const FlowNode*)> Walk = [&](const FlowNode* node) {
        if ((visited.count(node) > 0) || (stop_nodes.count(node) > 0)) {
            return;
        }
        visited.insert(node);

        tint::Switch(
            node,
            [&](const ir::Function* f) {
                out << "Function" << std::endl;

                {
                    ScopedIndent func_indent(&indent_size);
                    ScopedStopNode scope(&stop_nodes, f->end_target);
                    Walk(f->start_target);
                }
                Walk(f->end_target);
            },
            [&](const ir::Block* b) {
                indent() << "Block" << std::endl;
                Walk(b->branch_target);
            },
            [&](const ir::Switch* s) {
                indent() << "Switch" << std::endl;

                {
                    ScopedIndent switch_indent(&indent_size);
                    ScopedStopNode scope(&stop_nodes, s->merge_target);
                    for (const auto& c : s->cases) {
                        indent() << "Case" << std::endl;
                        ScopedIndent case_indent(&indent_size);
                        Walk(c.start_target);
                    }
                }

                indent() << "Switch Merge" << std::endl;
                Walk(s->merge_target);
            },
            [&](const ir::If* i) {
                indent() << "if" << std::endl;
                {
                    ScopedIndent if_indent(&indent_size);
                    ScopedStopNode scope(&stop_nodes, i->merge_target);

                    indent() << "If true" << std::endl;
                    Walk(i->true_target);

                    indent() << "If false" << std::endl;
                    Walk(i->false_target);
                }

                indent() << "if merge" << std::endl;
                Walk(i->merge_target);
            },
            [&](const ir::Loop* l) {
                indent() << "loop" << std::endl;
                {
                    ScopedStopNode loop_scope(&stop_nodes, l->merge_target);
                    ScopedIndent loop_indent(&indent_size);
                    {
                        ScopedStopNode inner_scope(&stop_nodes, l->continuing_target);
                        indent() << "loop start" << std::endl;
                        Walk(l->start_target);
                    }

                    indent() << "loop continuing" << std::endl;
                    ScopedIndent continuing_indent(&indent_size);
                    Walk(l->continuing_target);
                }

                indent() << "loop merge" << std::endl;
                Walk(l->merge_target);
            },
            [&](const ir::Terminator*) { indent() << "Function end" << std::endl; });
    };

    for (const auto* func : mod->functions) {
        Walk(func);
    }

    return out.str();
}

}  // namespace tint::ir
