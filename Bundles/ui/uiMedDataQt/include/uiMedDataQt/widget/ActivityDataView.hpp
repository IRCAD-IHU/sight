/************************************************************************
 *
 * Copyright (C) 2016-2020 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "uiMedDataQt/config.hpp"

#include <fwActivities/registry/Activities.hpp>

#include <fwData/Composite.hpp>

#include <fwMedData/ActivitySeries.hpp>

#include <QEvent>
#include <QPointer>
#include <QTabWidget>
#include <QTreeWidget>

namespace uiMedDataQt
{
namespace widget
{

/**
 * @brief This class displays a tab widget allowing to select the required data to create an activity.
 */
class UIMEDDATAQT_CLASS_API ActivityDataView : public QTabWidget
{
Q_OBJECT;

public:

    typedef std::map< std::string, std::string > ObjectIconMapType;

    UIMEDDATAQT_API ActivityDataView(QWidget* parent = 0);

    UIMEDDATAQT_API virtual ~ActivityDataView();

    /// Identifier of the role UID in the series tree item.
    UIMEDDATAQT_API static const int s_UID_ROLE;

    /// Defines comun header columns used in the tree widget of all created tabs.
    enum class ColumnCommunType : int
    {
        ID = 0
    };

    /// Defines header columns used in the tree widget of all data.
    enum class ColumnObjectType : int
    {
        DESC = 1,
    };

    /// Defines header columns used in the tree widget of all series.
    enum class ColumnSeriesType : int
    {
        NAME = 1,
        SEX,
        BIRTHDATE,

        MODALITY,
        MODALITY_DESC,

        STUDY_DESC,
        DATE,
        TIME,

        PATIENT_AGE
    };

    /**
     * @brief Creates all tabs from an activity information.
     *
     * One tab will be added for each activity parameter, if the type of the data is a @ref ::fwMedData::Series,
     * more informations will be displayed in the tree widget.
     *
     * @param _info the struct containing the activity configuration.
     */
    UIMEDDATAQT_API void fillInformation(const ::fwActivities::registry::ActivityInfo& _info);

    /// Fill widget from existing activity info (create on item by activity parameter)
    UIMEDDATAQT_API void fillInformation(const ::fwMedData::ActivitySeries::sptr& actSeries);

    /**
     * @brief Check if all the required data are selected and fill activity data composite.
     * @return True if data are correct, else false and errorMsg contains detailled error.
     * @param[in] actSeries activitySeries to fill with the selected data
     * @param[out] errorMsg it will contain the error information if the data are not correct.
     *
     * It checks if all required data are present in the widget, and create the activity data composite.
     * Then it checks if the activity parameters are accessible from the composite of data (it checks if the object are
     * accessible from the sesh@ paths.
     */
    UIMEDDATAQT_API bool checkAndComputeData(const ::fwMedData::ActivitySeries::sptr& actSeries,
                                             std::string& errorMsg);

    /**
     * @brief Check if the current required data are selected And return this data.
     * @return return the data if it is correct, else return a nullptr and errorMsg contains detailled error.
     * @param[in] index index of the requirement to check (this index also correspond to a tab).
     * @param[out] errorMsg it will contain the error information if the data are not correct.
     */
    UIMEDDATAQT_API ::fwData::Object::sptr checkData(size_t index, std::string& errorMsg);

    /// Set io selector configuration used to import data
    void setIOSelectorConfig(const std::string& ioSelectorSrvConfig)
    {
        m_ioSelectorSrvConfig = ioSelectorSrvConfig;
    }

    /// Set io selector configuration used to import data from a SDB
    void setSDBIOSelectorConfig(const std::string& ioSelectorSrvConfig)
    {
        m_sdbIoSelectorSrvConfig = ioSelectorSrvConfig;
    }

    /// Remove all the widget tabs.
    UIMEDDATAQT_API virtual void clear();

    /// Set the map that defines the icon associated to an object.
    void setObjectIconAssociation(const ObjectIconMapType& objectIcons)
    {
        m_objectIcons = objectIcons;
    }

protected:

    /// Catch KeyEvent and DropEvent from tree widgets
    bool eventFilter(QObject* obj, QEvent* event) override;

private:

    typedef std::vector< ::fwData::Object::csptr > ImportedObjectType;
    typedef std::vector<QPointer<QTreeWidget> > TreeContainerType;

    /// Import an object required for the selected tab
    void importObject();

    /// Import a SeriesDB and then extract the object required for the selected tab
    void importObjectFromSDB();

    /// Create a new object for the selected tab (just use ::fwData::factory::New(type))
    void createNewObject();

    /// Call SIOSelector service to read the object. Return true if the object is properly read.
    fwData::Object::sptr readObject(const std::string& classname, const std::string& ioSelectorSrvConfig);

    /// Removed the selected object in the current tree
    void removeSelectedObjects();

    /// Remove all the objects in the current tree
    void clearTree();

    /**
     * @brief Called on tree item double click : allows to edit the current data.
     * @note Currently it is only available for simple types (ie ::fwData::String)
     */
    void onTreeItemDoubleClicked(QTreeWidgetItem* item, int column);

    /**
     * @brief Add the current item in the tree.
     * @param index index used to find the associated tree widget
     * @param obj object to add in the tree
     */
    void addObjectItem(size_t index, const ::fwData::Object::csptr& obj);

    ::fwActivities::registry::ActivityInfo m_activityInfo; ///< activity information

    std::string m_ioSelectorSrvConfig; ///< io selector config
    std::string m_sdbIoSelectorSrvConfig; ///< io selector config to import data from a SeriesDB

    ImportedObjectType m_importedObject; ///< refereces on the imported before to add them in the activity series.
    TreeContainerType m_treeWidgets; ///< Register the tree widget of each tab.

    ObjectIconMapType m_objectIcons; ///< Map defining the icon associated to an object.
};

} // namespace widget
} // namespace uiMedDataQt
