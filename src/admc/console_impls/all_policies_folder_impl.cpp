/*
 * ADMC - AD Management Center
 *
 * Copyright (C) 2020-2022 BaseALT Ltd.
 * Copyright (C) 2020-2022 Dmitry Degtyarev
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "console_impls/all_policies_folder_impl.h"

#include "adldap.h"
#include "console_impls/item_type.h"
#include "console_impls/object_impl.h"
#include "console_impls/policy_impl.h"
#include "console_impls/policy_root_impl.h"
#include "console_widget/results_view.h"
#include "create_policy_dialog.h"
#include "globals.h"
#include "gplink.h"
#include "policy_results_widget.h"
#include "rename_policy_dialog.h"
#include "select_object_dialog.h"
#include "settings.h"
#include "status.h"
#include "utils.h"

#include <QCoreApplication>
#include <QDebug>
#include <QList>
#include <QMenu>
#include <QProcess>
#include <QStandardItem>

void all_policies_folder_impl_add_objects(ConsoleWidget *console, const QList<AdObject> &object_list, const QModelIndex &parent);

AllPoliciesFolderImpl::AllPoliciesFolderImpl(ConsoleWidget *console_arg)
: ConsoleImpl(console_arg) {
    set_results_view(new ResultsView(console_arg));

    create_policy_action = new QAction(tr("Create policy"), this);

    connect(
        create_policy_action, &QAction::triggered,
        this, &AllPoliciesFolderImpl::create_policy);
}

void AllPoliciesFolderImpl::fetch(const QModelIndex &index) {
    UNUSED_ARG(index);

    AdInterface ad;
    if (ad_failed(ad, console)) {
        return;
    }

    const QString base = g_adconfig->domain_dn();
    const SearchScope scope = SearchScope_All;
    const QString filter = filter_CONDITION(Condition_Equals, ATTRIBUTE_OBJECT_CLASS, CLASS_GP_CONTAINER);
    const QList<QString> attributes = console_policy_search_attributes();
    const QHash<QString, AdObject> results = ad.search(base, scope, filter, attributes);

    all_policies_folder_impl_add_objects(console, results.values(), index);
}

void AllPoliciesFolderImpl::refresh(const QList<QModelIndex> &index_list) {
    const QModelIndex index = index_list[0];

    console->delete_children(index);
    fetch(index);
}

QList<QAction *> AllPoliciesFolderImpl::get_all_custom_actions() const {
    QList<QAction *> out;

    out.append(create_policy_action);

    return out;
}

QSet<QAction *> AllPoliciesFolderImpl::get_custom_actions(const QModelIndex &index, const bool single_selection) const {
    UNUSED_ARG(index);
    UNUSED_ARG(single_selection);

    QSet<QAction *> out;

    out.insert(create_policy_action);

    return out;
}

QSet<StandardAction> AllPoliciesFolderImpl::get_standard_actions(const QModelIndex &index, const bool single_selection) const {
    UNUSED_ARG(index);
    UNUSED_ARG(single_selection);

    QSet<StandardAction> out;

    out.insert(StandardAction_Refresh);

    return out;
}

QList<QString> AllPoliciesFolderImpl::column_labels() const {
    return {tr("Name")};
}

QList<int> AllPoliciesFolderImpl::default_columns() const {
    return {0};
}

void AllPoliciesFolderImpl::create_policy() {
    AdInterface ad;
    if (ad_failed(ad, console)) {
        return;
    }

    const QList<QModelIndex> selected_list = console->get_selected_items(ItemType_AllPoliciesFolder);

    if (selected_list.isEmpty()) {
        return;
    }

    const QModelIndex parent_index = selected_list[0];

    auto dialog = new CreatePolicyDialog(ad, console);
    dialog->open();

    connect(
        dialog, &QDialog::accepted,
        this,
        [this, dialog, parent_index]() {
            AdInterface ad2;
            if (ad_failed(ad2, console)) {
                return;
            }

            const QString dn = dialog->get_created_dn();
            all_policies_folder_impl_add_objects_from_dns(console, ad2, {dn}, parent_index);
        });
}

QModelIndex get_all_policies_folder_index(ConsoleWidget *console) {
    const QModelIndex policy_tree_root = get_policy_tree_root(console);
    const QList<QModelIndex> index_list = console->search_items(policy_tree_root, {ItemType_AllPoliciesFolder});

    if (!index_list.isEmpty()) {
        return index_list[0];
    } else {
        return QModelIndex();
    }
}

void all_policies_folder_impl_add_objects_from_dns(ConsoleWidget *console, AdInterface &ad, const QList<QString> &dn_list, const QModelIndex &parent) {
    const QList<AdObject> object_list = [&]() {
        const QString base = g_adconfig->policies_dn();
        const SearchScope scope = SearchScope_Children;
        const QString filter = filter_dn_list(dn_list);
        const QList<QString> attributes = QList<QString>();

        const QHash<QString, AdObject> search_results = ad.search(base, scope, filter, attributes);

        const QList<AdObject> out = search_results.values();

        return out;
    }();

    all_policies_folder_impl_add_objects(console, object_list, parent);
}

void all_policies_folder_impl_add_objects(ConsoleWidget *console, const QList<AdObject> &object_list, const QModelIndex &parent) {
    if (!parent.isValid()) {
        return;
    }

    const bool parent_was_fetched = console_item_get_was_fetched(parent);
    if (!parent_was_fetched) {
        return;
    }

    for (const AdObject &object : object_list) {
        const QList<QStandardItem *> row = console->add_scope_item(ItemType_Policy, parent);

        console_policy_load(row, object);
    }
}
