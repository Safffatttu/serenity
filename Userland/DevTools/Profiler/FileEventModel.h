/*
 * Copyright (c) 2022, Jakub Berkop <jakub.berkop@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/LexicalPath.h>
#include <AK/String.h>
#include <LibGUI/Model.h>

namespace Profiler {

class Profile;

class FileEventNode : public RefCounted<FileEventNode> {
public:
    static NonnullRefPtr<FileEventNode> create(String const& path, FileEventNode* parent = nullptr)
    {
        return adopt_ref(*new FileEventNode(path, parent));
    }

    FileEventNode(String const& path, FileEventNode* parent = nullptr)
        : m_path(path)
        , count(1)
        , m_parent(parent) {
            // dbgln("Created this:{} parent:{} path:{}", this, parent, full_path());
        };

    Optional<FileEventNode*> find_node_or_extend_recursive(String);

    Vector<NonnullRefPtr<FileEventNode>> const& children() { return m_children; };
    Vector<NonnullRefPtr<FileEventNode>> const& children() const { return m_children; };

    FileEventNode* parent() { return m_parent; };

    NonnullRefPtr<FileEventNode> create_recursively(String);

    String full_path()
    {
        if (m_parent) {
            return String::formatted("{}{}", m_parent->m_path, m_path);
        } else {
            return m_path;
        }
    }

    void dump_recursive(int level = 0);

    String m_path;
    u64 count;
    Vector<NonnullRefPtr<FileEventNode>> m_children;
    FileEventNode* m_parent = nullptr;
};

class FileEventModel final : public GUI::Model {
public:
    static NonnullRefPtr<FileEventModel> create(Profile& profile)
    {
        return adopt_ref(*new FileEventModel(profile));
    }

    enum Column {
        Path,
        Count,
        __Count
    };

    virtual ~FileEventModel() override;

    virtual int row_count(GUI::ModelIndex const& = GUI::ModelIndex()) const override;
    virtual int column_count(GUI::ModelIndex const& = GUI::ModelIndex()) const override;
    virtual String column_name(int) const override;
    virtual GUI::Variant data(GUI::ModelIndex const&, GUI::ModelRole) const override;
    virtual GUI::ModelIndex index(int row, int column, GUI::ModelIndex const& parent = GUI::ModelIndex()) const override;
    virtual GUI::ModelIndex parent_index(GUI::ModelIndex const&) const override;
    virtual int tree_column() const override { return Column::Path; }
    virtual bool is_column_sortable(int) const override { return false; }
    virtual bool is_searchable() const override { return true; }
    virtual Vector<GUI::ModelIndex> matches(StringView, unsigned flags, GUI::ModelIndex const&) override;

private:
    explicit FileEventModel(Profile&);

    Profile& m_profile;

    GUI::Icon m_user_frame_icon;
    GUI::Icon m_kernel_frame_icon;
};

}
