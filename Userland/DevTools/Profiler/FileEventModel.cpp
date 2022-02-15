/*
 * Copyright (c) 2022, Jakub Berkop <jakub.berkop@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "FileEventModel.h"
#include "Profile.h"
#include <AK/StringBuilder.h>
#include <LibGUI/FileIconProvider.h>
#include <LibSymbolication/Symbolication.h>
#include <stdio.h>

namespace Profiler {

FileEventModel::FileEventModel(Profile& profile)
    : m_profile(profile)
{
}

FileEventModel::~FileEventModel()
{
}

void FileEventNode::dump_recursive(int level)
{
    StringBuilder sb;
    for (int i = 0; i < level; i++)
    {
        sb.append("    ");
    }

    dbgln("{}->{}  {}{}:{}", this, m_parent, sb.to_string(), m_path, count);
    for (auto& child: m_children)
    {
        child->dump_recursive(level + 1);
    }
}


Optional<FileEventNode*> FileEventNode::find_node_or_extend_recursive(String searched_path) // A/B/C
{
    dbgln("find_node_or_extend_recursive {} {}", searched_path, full_path());

    if (searched_path == ""sv)
    {
        dbgln("Found me hehe");
        return this;
    }

    auto lex_path = LexicalPath(searched_path);
    auto parts = lex_path.parts(); // { A B C.aa }
    auto current = parts.take_first();

    StringBuilder sb;
    sb.join("/", parts);
    auto new_s = sb.to_string();

    for (auto& child : m_children) {
        dbgln("Comparing {} with {}", child->m_path, current);
        if (child->m_path == current) {
            dbgln("Went there");
            return child->find_node_or_extend_recursive(new_s);
        }
    }

    dbgln("Not found anything LETZ GO BOIZ");

    if(m_parent)
    {
        for (auto& child : m_children) {
            dbgln("22222222 {} with {}", child->m_path, current);
            if (child->m_path == current) {
                return child->find_node_or_extend_recursive(new_s);
            }
        }

        auto new_child = create_recursively(searched_path);
        return new_child.ptr();
    }
    else
    {
        if (!searched_path.starts_with("/"sv) && !m_parent) {
            dbgln("Creating node for special path {}", searched_path);
            auto new_child = create(searched_path, this);
            m_children.append(new_child);
            return new_child.ptr();
        }

        return create_recursively(searched_path);
    }
}

NonnullRefPtr<FileEventNode> FileEventNode::create_recursively(String new_path)
{
    dbgln("Creating recurse {}", new_path);
    auto lex_path = LexicalPath(new_path);
    auto parts = lex_path.parts(); // { A B C.aa }

    if (parts.size() == 1) {
        dbgln("Create leaf {} {}", new_path, this);
        auto new_node = FileEventNode::create(new_path, this);
        m_children.append(new_node);
        return new_node;
    } else {
        auto last = parts.take_first();
        dbgln("Create node {} {}", last, this);
        auto new_node = FileEventNode::create(last, this);
        m_children.append(new_node);

        StringBuilder sb;
        sb.join("/", parts);
        auto new_s = sb.to_string();

        return new_node->create_recursively(new_s);
    }
}

GUI::ModelIndex FileEventModel::index(int row, int column, GUI::ModelIndex const& parent) const
{
    // dbgln("index  {}:{}", row, column);
    if (!parent.is_valid()) {
        // dbgln("index parent invalid {}:{}", row, column);
        // if (m_profile.file_event_nodes().is_empty())
        //     return {};
        return create_index(row, column, m_profile.file_event_nodes()->children()[row].ptr());
    }
    auto& remote_parent = *static_cast<FileEventNode*>(parent.internal_data());
    // remote_parent.dump_recursive();
    // dbgln("index paren: {} {}:{}", &remote_parent, row, column);
    return create_index(row, column, remote_parent.children().at(row).ptr());
}

GUI::ModelIndex FileEventModel::parent_index(GUI::ModelIndex const& index) const
{
    // dbgln("parent_index {}", index);
    if (!index.is_valid())
        return {};
    auto& node = *static_cast<FileEventNode*>(index.internal_data());
    if (!node.parent())
        return {};

    #if 1
    // dbgln("we have parent {}", node.parent());

    if(node.parent()->parent())
    {
        // auto parent_parent = node.parent()->parent();
        auto& children = node.parent()->parent()->children();

        for (size_t row = 0; row < children.size(); ++row) {
            // dbgln("TryingP{}", children.at(row).ptr());
            if (children.at(row).ptr() == node.parent())
            {
                return create_index(row, index.column(), node.parent());
            }
        }
    }


    auto& children = node.parent()->children();

    for (size_t row = 0; row < children.size(); ++row) {
        // dbgln("Trying {}", children.at(row).ptr());
        if (children.at(row).ptr() == &node)
        {
            return create_index(row, index.column(), node.parent());
        }
    }
    #endif

    VERIFY_NOT_REACHED();
    return {};
}

int FileEventModel::row_count(GUI::ModelIndex const& index) const
{
    if (!index.is_valid())
        return 1;
    auto& node = *static_cast<FileEventNode*>(index.internal_data());
    return node.children().size();
}

int FileEventModel::column_count(GUI::ModelIndex const&) const
{
    return Column::__Count;
}

String FileEventModel::column_name(int column) const
{
    switch (column) {
    case Column::Path:
        return "Path";
    case Column::Count:
        return "Count";
    default:
        VERIFY_NOT_REACHED();
        return {};
    }
}

GUI::Variant FileEventModel::data(GUI::ModelIndex const& index, GUI::ModelRole role) const
{
    if (role == GUI::ModelRole::TextAlignment) {
        if(index.column() == Path)
            return Gfx::TextAlignment::CenterLeft;
        return Gfx::TextAlignment::CenterRight;
    }

    auto* node = static_cast<FileEventNode*>(index.internal_data());

    /*
    if (role == GUI::ModelRole::TextAlignment) {
        if (index.column() == Column::SampleCount || index.column() == Column::SelfCount)
            return Gfx::TextAlignment::CenterRight;
    }
    if (role == GUI::ModelRole::Icon) {
        if (index.column() == Column::StackFrame) {
            if (node->is_root()) {
                return GUI::FileIconProvider::icon_for_executable(node->process().executable);
            }
            auto maybe_kernel_base = Symbolication::kernel_base();
            if (maybe_kernel_base.has_value() && node->address() >= maybe_kernel_base.value())
                return m_kernel_frame_icon;
            return m_user_frame_icon;
        }
        return {};
    }*/

    if (role == GUI::ModelRole::Display) {
        // dbgln("Asked column {} for {}:{}", index.column(), node->m_path, node->count);
        if (index.column() == Column::Count) {
            // dbgln("Asked for row {} for {}:{}", index.row(), node->m_path, node->count);
            return node->count;
        }

        if (index.column() == Column::Path) {
            return node->m_path;
        }

        return {};
    }

    return {};
}

Vector<GUI::ModelIndex> FileEventModel::matches(StringView searching, unsigned flags, GUI::ModelIndex const& parent)
{
    dbgln("MATCHES??");
    (void)flags;
    (void)parent;
    (void)searching;
    // RemoveReference<decltype(m_profile.file_event_nodes())>* nodes { nullptr };

    // if (!parent.is_valid())
    //     nodes = &m_profile.file_event_nodes();
    // else
    //     nodes = &static_cast<FileEventNode*>(parent.internal_data())->children();

    Vector<GUI::ModelIndex> found_indices;
    // for (auto it = nodes->begin(); !it.is_end(); ++it) {
    //     GUI::ModelIndex index = this->index(it.index(), Path, parent);
    //     if (!string_matches(data(index, GUI::ModelRole::Display).as_string(), searching, flags))
    //         continue;

    //     found_indices.append(index);
    //     if (flags & FirstMatchOnly)
    //         break;
    // }

    return found_indices;
}

}
