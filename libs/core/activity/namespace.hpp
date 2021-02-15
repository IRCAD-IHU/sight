/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

/**
 * @brief Namespace containing activity data and builder
 */
#pragma once

namespace sight::activity
{

/// The namespace sightactivity::builder contains the base interface for data ActivitySeries builder.
namespace builder
{

/// Contains activity::builder::registry details
namespace registry
{
} // namespace registry

/// Contains activity::builder::factory utilities
namespace factory
{
} // namespace factory

} // end namespace builder

namespace validator
{

/// Contains activity::validator::registry details
namespace registry
{
} // namespace registry

/// Contains activity::validator::factory utilities
namespace factory
{
} // namespace factory

} // namespace validator

/**
 * @page Activity Activity configuration
 *
 * An activity is defined by the extension "::sight::activity::registry::Activity". It is used to launch an
 * AppConfig with the selected data, it will create a new data data::ActivitySeries that inherits from a
 * data::Series.
 *
 * The service activity::action::SActivityLauncher allows to launch an activity. Its role is to create the specific
 * Activity associated with the selected data.
 *
 * This action should be followed by the service modules::ui::qt::editor::SDynamicView : this service listens the action
 * signals
 * and launchs the activity in a new tab.
 *
 * - activity::action::SActivityLauncher uses the selected data to generate the activity.
 * - modules::ui::qt::editor::SDynamicView  displays the activity in the application.
 * - data::Vector contains the set of selected data .
 *
 * @code{.xml}
     <extension implements="::sight::activity::registry::Activity">
         <id>myActivityId</id>
         <title>3D Visu</title>
         <desc>Activity description ...</desc>
         <icon>media-0.1/icons/icon-3D.png</icon>
         <requirements>
            <requirement name="param1" type="data::Image" /> <!-- defaults : minOccurs = 1, maxOccurs = 1-->
            <requirement name="param2" type="data::Mesh" maxOccurs="3" >
                <key>Item1</key>
                <key>Item2</key>
                <key>Item3</key>
            </requirement>
            <requirement name="param3" type="data::Mesh" maxOccurs="*" container="vector" />
            <requirement name="imageSeries" type="data::ImageSeries" minOccurs="0" maxOccurs="2" />
            <requirement name="modelSeries" type="data::ModelSeries" minOccurs="1" maxOccurs="1">
                 <desc>Description of the required data....</desc>
                 <validator>activity::validator::ImageProperties</validator>
            </requirement>
            <requirement name="transformationMatrix" type="data::TransformationMatrix3D" minOccurs="0" maxOccurs="1"
 * create="true" />
             <!-- ...-->
         </requirements>
         <builder>activity::builder::ActivitySeries</builder>
         <validator>activity::validator::ImageProperties</validator>
         <appConfig id="myAppConfigId">
            <parameters>
                <parameter replace="registeredImageUid" by="@values.param1" />
                <parameter replace="orientation" by="frontal" />
                <!-- ...-->
            </parameters>
         </appConfig>
     </extension>
   @endcode
 *
 * The activity parameters are (in this order):
 *
 * - \b id: it is the activity unique identifier.
 * - \b title: it is the activity title that will be displayed on the tab.
 * - \b desc: it is the description of the activity. It is displayed by the SActivityLauncher when several activity can
 *   be launched with the selected data.
 * - \b icon: it is the path to the activity icon. It is displayed by the SActivityLauncher when several activity can be
 *   launched with the selected data.
 * - \b requirements: it is the list of the data required to launch the activity.
 *     - \b requirement: a required data.
 *         - \b name: key used to add the data in the activity Composite.
 *         - \b type: the data type (ex: ``data::ImageSeries``).
 *         - \b minOccurs (optional, "1" by default): the minimum number of occurrences of this type of object in the
 *           vector.
 *         - \b maxOccurs (optional, "1" by default): the maximum number of occurrences of this type of object in the
 *           vector.
 *         - \b container (optional, "vector" or "composite", default: composite): container used to contain the data if
 *           minOccurs or maxOccurs are not "1". if the container is "composite", you need to specify the "key" of each
 *           object in the composite.
 *         - \b create (optional, default "false"): if true and (minOccurrs == 0 && maxOccurs == 1), the data will be
 *           automatically created if it is not present.
 *         - \b desc (optional): description of the parameter
 *         - \b validator (optional): validator to check if the associated data is well formed (inherited of
 *           ::fwAtivities::IObjectValidator)
 * - \b builder: implementation of the activity builder. The default builder is
 *   ``activity::builder::ActivitySeries`` : it creates the ``data::ActivitySeries`` and adds the required
 *   data in its composite with de defined key.
 *   The builder ``activity::builder::ActivitySeriesInitData`` allows, in addition to what the default builder
 *   does, to create data when minOccurs == 0 et maxOccurs == 0.
 * - \b validators (optional):  it defines the list of validator. If you need only one validator, you don't need the
 *   "validators" tag (only "validator").
 * - \b validator (optional): it allows to validate if the selected required object are correct for the activity.
 *   For example, the validator ``activity::validator::ImageProperties`` checks that all the selected images
 *   have the same size, spacing and origin.
 * - \b appConfig: it defines the AppConfig to launch and its parameters
 * - \b id: identifier of the AppConfig
 * - \b parameters: list of the parameters required by the AppConfig
 *    - \b parameter: defines a parameter
 *          - \b replace: name of the parameter as defined in the AppConfig
 *          - \b by: defines the string that will replace the parameter name. It should be a simple string (ex.
 *               frontal) or define a sesh@ path (ex. @values.myImage). The root object of the sesh@ path if the
 *               composite contained in the ActivitySeries.
 *
 * @section Validators Validators
 *
 * There is three types of validator :
 *
 * @subsection prebuild Pre-build validator
 *
 * This type of validators checks if the current selection of data is correct to build the activity. It inherits of
 * activity::IValidator and must implement the methods:
 *
 * @code {.cpp}
   ValidationType validate(
           const activity::registry::ActivityInfo& activityInfo,
           SPTR(data::Vector) currentSelection ) const;
   @endcode
 *
 * @subsection activity Activity validator
 *
 * This type of validator checks if the data::ActivitySeries is correct to launch its associated activity.
 * It inherits of activity::IActivityValidator and must implement the method:
 *
 * @code {.cpp}
   ValidationType validate(const CSPTR(data::ActivitySeries) &activity ) const;
   @endcode
 *
 * The validator activity::validator::DefaultActivity is applied if no other validator is defined. It checks if
 * all the required objets are present in the series and if all the parameters delivered to the AppConfig are present.
 *
 * It provides some method useful to implement your own validator.
 *
 * @subsection object Object validator
 *
 * This type of validator checks if the required object is well formed. It can check a single object or a Vector or
 * a Composite containing one type of object. It inherits of activity::IObjectValidator and must implement the
 * method:
 *
 * @code {.cpp}
   ValidationType validate(const CSPTR(data::Object) &currentData ) const;
   @endcode
 */

} // namespace sight::activity
