/*
 * ADMC - AD Management Center
 *
 * Copyright (C) 2020 BaseALT Ltd.
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

#include "object_context_menu.h"
#include "ad_interface.h"
#include "ad_config.h"
#include "confirmation_dialog.h"
#include "select_dialog.h"
#include "rename_dialog.h"
#include "password_dialog.h"
#include "create_dialog.h"
#include "details_dialog.h"
#include "utils.h"

#include <QPoint>
#include <QAbstractItemView>

// Open this context menu when view requests one
void ObjectContextMenu::connect_view(QAbstractItemView *view, int dn_column) {
    QObject::connect(
        view, &QWidget::customContextMenuRequested,
        [=]
        (const QPoint pos) {
            const QString dn = get_dn_from_pos(pos, view, dn_column);
            if (dn.isEmpty()) {
                return;
            }

            ObjectContextMenu context_menu(dn);
            exec_menu_from_view(&context_menu, view, pos);
        });
}

ObjectContextMenu::ObjectContextMenu(const QString &dn)
: QMenu()
{
    const AdObject object = AD()->search_object(dn);

    addAction(tr("Details"), [this, dn]() {
        DetailsDialog::open_for_target(dn);
    });

    auto delete_action = addAction(tr("Delete"), [this, dn, object]() {
        delete_object(dn, object);
    });

    auto rename_action = addAction(tr("Rename"), [this, dn]() {
        auto dialog = new RenameDialog(dn);
        dialog->open();
    });

    QMenu *submenu_new = addMenu("New");
    static const QList<QString> create_classes = {
        CLASS_USER,
        CLASS_COMPUTER,
        CLASS_OU,
        CLASS_GROUP,
    };
    for (const auto object_class : create_classes) {
        const QString action_text = ADCONFIG()->get_class_display_name(object_class);

        submenu_new->addAction(action_text, [dn, object_class]() {
            const auto create_dialog = new CreateDialog(dn, object_class);
            create_dialog->open();
        });
    }

    auto move_action = addAction(tr("Move"));
    connect(
        move_action, &QAction::triggered,
        [this, dn, object]() {
            move(dn, object);
        });

    if (object.is_class(CLASS_USER)) {
        QAction *add_to_group_action = addAction(tr("Add to group"));
        connect(
            add_to_group_action, &QAction::triggered,
            [this, dn]() {
                add_to_group(dn);
            });

        addAction(tr("Reset password"), [dn]() {
            const auto password_dialog = new PasswordDialog(dn);
            password_dialog->open();
        });

        const bool disabled = object.get_account_option(AccountOption_Disabled);
        QString disable_text;
        if (disabled) {
            disable_text = tr("Enable account");
        } else {
            disable_text = tr("Disable account");
        }
        addAction(disable_text, [this, dn, disabled]() {
            AD()->user_set_account_option(dn, AccountOption_Disabled, !disabled);
        });
    }

    // Disable certain actions based on system flags
    if (object.contains(ATTRIBUTE_SYSTEM_FLAGS)) {
        const bool cannot_move = object.get_system_flag(SystemFlagsBit_CannotMove);
        const bool cannot_rename = object.get_system_flag(SystemFlagsBit_CannotRename);
        const bool cannot_delete = object.get_system_flag(SystemFlagsBit_CannotDelete);
        
        if (cannot_move) {
            move_action->setEnabled(false);
        }
        if (cannot_delete) {
            delete_action->setEnabled(false);
        }
        if (cannot_rename) {
            rename_action->setEnabled(false);
        }
    }
}

void ObjectContextMenu::delete_object(const QString &dn, const AdObject &object) {
    const QString name = object.get_string(ATTRIBUTE_NAME);
    const QString text = QString(tr("Are you sure you want to delete \"%1\"?")).arg(name);
    const bool confirmed = confirmation_dialog(text, this);

    if (confirmed) {
        AD()->object_delete(dn);
    }    
}

void ObjectContextMenu::move(const QString &dn, const AdObject &object) {
    const QList<QString> object_classes = object.get_strings(ATTRIBUTE_OBJECT_CLASS);
    const QList<QString> possible_superiors = ADCONFIG()->get_possible_superiors(object_classes);

    const QList<QString> selected_objects = SelectDialog::open(possible_superiors);

    if (selected_objects.size() == 1) {
        const QString container = selected_objects[0];

        AD()->object_move(dn, container);
    }
}

void ObjectContextMenu::add_to_group(const QString &dn) {
    const QList<QString> classes = {CLASS_GROUP};
    const QList<QString> selected_objects = SelectDialog::open(classes, SelectDialogMultiSelection_Yes);

    if (selected_objects.size() > 0) {
        for (auto group : selected_objects) {
            AD()->group_add_user(group, dn);
        }
    }
}
