/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#pragma once

#include "fwGuiQml/config.hpp"

#include <fwGui/dialog/ISelectorDialog.hpp>

#include <QObject>
#include <QVector>

#include <vector>

namespace fwGuiQml
{
namespace dialog
{
//------------------------------------------------------------------------------

/**
 * @brief   SelectorDialog allowing the choice of an element among severals (_selections)
 */
class FWGUIQML_CLASS_API SelectorDialog : public QObject,
                                          public ::fwGui::dialog::ISelectorDialog
{
Q_OBJECT
Q_PROPERTY(QString title MEMBER m_title NOTIFY titleChanged)
Q_PROPERTY(QString message MEMBER m_message NOTIFY messageChanged)
Q_PROPERTY(bool visible MEMBER m_visible)
public:

    fwCoreClassDefinitionsWithFactoryMacro( (SelectorDialog)(::fwGui::dialog::ISelectorDialog),
                                            (()),
                                            ::fwGui::factory::New< SelectorDialog > );

    FWGUIQML_API SelectorDialog(::fwGui::GuiBaseObject::Key key);

    FWGUIQML_API virtual ~SelectorDialog();

    /**
     * @brief The string list that can be chosen by the selector.
     */
    FWGUIQML_API virtual void setSelections(std::vector< std::string > _selections) override;

    /**
     * @brief Sets the selector title.
     */
    FWGUIQML_API virtual void setTitle(std::string _title) override;

    /**
     * @brief Show the selector and return the selection.
     */
    FWGUIQML_API virtual std::string show() override;

    /// Set the message
    FWGUIQML_API virtual void setMessage(const std::string& msg) override;

    /// uninplemented
    FWGUIQML_API virtual void addCustomButton(const std::string& label, std::function<void()> clickedFn) override;

Q_SIGNALS:
    void titleChanged();
    void messageChanged();

protected Q_SLOTS:
    void resultDialog(QVariant selection);

private:
    std::vector< std::string > m_selections;

    /// Dialog box message
    QString m_message;
    QString m_title;
    QString m_selection;

    bool m_visible;
    bool m_isClicked;
};

} // namespace dialog
} // namespace fwGuiQt
