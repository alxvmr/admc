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

#ifndef SCOPE_MODEL_H
#define SCOPE_MODEL_H

#include "console_widget/console_drag_model.h"

/**
 * Implements showing expander for unfetched items.
 */

class ScopeModel : public ConsoleDragModel {
Q_OBJECT

public:
    using ConsoleDragModel::ConsoleDragModel;

    bool hasChildren(const QModelIndex &parent) const override;
};

#endif /* SCOPE_MODEL_H */
