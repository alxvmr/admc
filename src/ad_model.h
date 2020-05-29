
#ifndef AD_MODEL_H
#define AD_MODEL_H

#include "ad_interface.h"

#include <QStandardItemModel>

QString get_dn_of_index(const QModelIndex &index);

class AdModel final : public QStandardItemModel {
Q_OBJECT

public:
    enum Column {
        Name,
        Category,
        Description,
        DN,
        COUNT,
    };

    enum Roles {
        AdvancedViewOnly = Qt::UserRole + 1,
        CanFetch = Qt::UserRole + 2,
        IsContainer = Qt::UserRole + 3,
    };

    explicit AdModel(QObject *parent);

    bool canFetchMore(const QModelIndex &parent) const;
    void fetchMore(const QModelIndex &parent);
    bool hasChildren(const QModelIndex &parent) const override;

    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;

private slots:
    void on_load_attributes_complete(const QString &dn);
    void on_delete_entry_complete(const QString &dn); 
    void on_move_user_complete(const QString &user_dn, const QString &container_dn, const QString &new_dn);
    void on_create_entry_complete(const QString &dn, NewEntryType type); 

private:

};

#endif /* AD_MODEL_H */
