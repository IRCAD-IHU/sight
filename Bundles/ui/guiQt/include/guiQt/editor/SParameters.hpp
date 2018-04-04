/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "guiQt/config.hpp"

#include <fwGui/editor/IEditor.hpp>

#include <QGridLayout>
#include <QLabel>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QSignalMapper>
#include <QSlider>

namespace guiQt
{

namespace editor
{

/**
 * @brief Generic editor to interact with parameters.
 *
 * It allows to setup an editor with several parameters.
 * Once the user validates the parameters, a signal is sent containing the key and the value.
 * It supports booleans, doubles or integer at the moment.
 *
 * @note This service doesn't need any data.
 *
 * @section Signals Signals
 * - \b boolChanged(bool, std::string): Emitted when an boolean parameter changes.
 * - \b colorChanged(std::array<std::uint8_t, 4>, std::string): Emitted when a color parameter changes.
 * - \b doubleChanged(double, std::string): Emitted when a real parameter changes.
 * - \b double2Changed(double, double, std::string): Emitted when two real parameters change.
 * - \b double3Changed(double, double, double, std::string): Emitted when three real parameters change.
 * - \b intChanged(int, std::string): Emitted when an integer parameter changes.
 * - \b int2Changed(int, int, std::string): Emitted when two integer parameters change.
 * - \b int3Changed(int, int, int, std::string): Emitted when three integer parameters change.
 * - \b enumChanged(std::string, std::string): Emitted when enum parameter changes.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." type="::guiQt::editor::SParameters" >
        <parameters>
            <param type="bool" name="boolean parameter" key="boolParam" defaultValue="false" />
            <param type="double" name="real parameter" key="doubleParam" defaultValue="" min="1.5" max="42.42" />
            <param type="int" name="integer parameter" key="intParam" defaultValue="1" min="0" max="255" />
            <param type="enum" name="enum parameters" key="enumParam" defaultValue="p1" values="p1,p2,p3" />
        </parameters>
       </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b type: bool, color, double, double2, double3, int, int2, int3.
 * - \b name: label to display.
 * - \b key: name used in the signal to identify the parameter.
 * - \b defaultValue: value used to initialize the parameter.
 * - \b min: minimum value, if relevant for the data type.
 * - \b max: maximum value, if relevant for the data type.
 * - \b widget (optional) : widget type, available for types 'int' and 'double'.
 * You can choose between a 'spin' or a 'slider' widget. Defaults to 'spin' for 'double' and  'slider' for 'int'.
 * - \b decimals (optional, default=2): number of decimals settable using a double slider.
 * - \b values: list of possible values separated by a comma ',' a space ' ' or a semicolon ';' (only for enum type).
 * The actual displayed value and the returned one in the signal can be different using '=' to separate the two. For
 * example 'values="BLEND=imageBlend,CHECKERBOARD=imageCheckerboard"' means the combo will display BLEND, CHECKBOARD
 * and will send 'imageBlend' or 'imageCheckerboard'.
 */
class GUIQT_CLASS_API SParameters : public QObject,
                                    public ::fwGui::editor::IEditor
{
Q_OBJECT
public:
    fwCoreServiceClassDefinitionsMacro( (SParameters)(::fwGui::editor::IEditor) );

    /// Boolean changed signal type
    typedef ::fwCom::Signal< void (bool, std::string) > BooleanChangedSignalType;

    /// Color changed signal type
    typedef ::fwCom::Signal< void (std::array<std::uint8_t, 4>, std::string) > ColorChangedSignalType;

    /// Double changed signal type
    typedef ::fwCom::Signal< void (double, std::string) > DoubleChangedSignalType;
    typedef ::fwCom::Signal< void (double, double, std::string) > Double2ChangedSignalType;
    typedef ::fwCom::Signal< void (double, double, double, std::string) > Double3ChangedSignalType;

    /// Integer changed signal type
    typedef ::fwCom::Signal< void (int, std::string) > IntegerChangedSignalType;
    typedef ::fwCom::Signal< void (int, int, std::string) > Integer2ChangedSignalType;
    typedef ::fwCom::Signal< void (int, int, int, std::string) > Integer3ChangedSignalType;

    /// Enum changed signal type
    typedef ::fwCom::Signal< void (std::string, std::string) > EnumChangedSignalType;

    /// Constructor. Initializes signals
    GUIQT_API SParameters() noexcept;

    /// Destructor. Does nothing
    GUIQT_API virtual ~SParameters() noexcept;

    /// Configure the editor.
    GUIQT_API void configuring() override;

    /// Initializes Qt input widgets for parameters according to xml configuration
    GUIQT_API void starting() override;

    /// This method launches the IEditor::stopping method
    GUIQT_API void stopping() override;

    /// This method is used to update services. Does nothing
    GUIQT_API void updating() override;

private Q_SLOTS:

    /// This method is called when a boolean value changes
    void onChangeBoolean(int value);

    /// This method is called when a color button is clicked
    void onColorButton();

    /// This method is called when an integer value changes
    void onChangeInteger(int value);

    /// This method is called when a double value changes
    void onChangeDouble(double value);

    /// This method is called when a double slider value changes
    void onChangeDoubleSlider(int value);

    /// This method is called when selection changes (QComboBox)
    void onChangeEnum(int value);

    /// This method is called to connect sliders to their labels
    void onSliderMapped(QWidget* widget);

    /// This method is called to connect double sliders to their labels
    void onDoubleSliderMapped(QWidget* widget);

    /// This method is called to connect reset buttons and checkboxes
    void onResetBooleanMapped(QWidget* widget);

    /// This method is called to connect reset buttons and color widgets
    void onResetColorMapped(QWidget* widget);

    /// This method is called to connect reset buttons and sliders
    void onResetIntegerMapped(QWidget* widget);

    /// This method is called to connect reset buttons and sliders
    void onResetDoubleMapped(QWidget* widget);

private:

    /// Snippet to create the reset button
    QPushButton* createResetButton();

    /// Create a widget associated with a boolean type
    void createBoolWidget(QGridLayout& layout, int row, const std::string& key, const std::string& defaultValue);

    /// Create a widget associated with a color type
    void createColorWidget(QGridLayout& layout, int row, const std::string& key, const std::string& defaultValue);

    /// Create a widget associated with a double type
    void createDoubleWidget(QGridLayout& layout, int row, const std::string& key, double defaultValue,
                            double min, double max, int count);

    /// Create a slider widget associated with a double type.
    void createDoubleSliderWidget(QGridLayout& layout, int row, const std::string& key, double defaultValue,
                                  double min, double max, std::uint8_t decimals);

    /// Create a slider widget associated with an integer type
    void createIntegerSliderWidget(QGridLayout& layout, int row, const std::string& key,
                                   int defaultValue, int min, int max);

    /// Create a spin widget associated with an integer type
    void createIntegerSpinWidget(QGridLayout& layout, int row, const std::string& key,
                                 int defaultValue, int min, int max, int count);

    /// Create a multi choice widget
    void createEnumWidget(QGridLayout& layout, int row, const std::string& key, const std::string& defaultValue,
                          const std::vector< std::string>& values, const std::vector<std::string>& data);

    /// Emit the signal(s) for the integer widget
    void emitIntegerSignal(QObject* widget);

    /// Emit the signal(s) for the double widget
    void emitDoubleSignal(QObject* spinbox);

    /// Computes a double slider value from a slider position.
    static double getDoubleSliderValue(const QSlider* slider);

    /// Adjust the minimum size of a label according to the range values
    template <typename T>
    static void setLabelMinimumSize(QLabel* label, T min, T max, std::uint8_t decimals = 0);

    template<typename T>
    static QString valueToStringLabel(T value, std::uint8_t decimals);

    /// Allows to connect sliders and their labels
    QPointer< QSignalMapper> m_integerSliderSignalMapper;
    QPointer< QSignalMapper> m_doubleSliderSignalMapper;
    QPointer< QSignalMapper> m_resetMapper;
};

//------------------------------------------------------------------------------

template<> inline QString SParameters::valueToStringLabel<int>(int value, std::uint8_t)
{
    return QString("%1").arg(value);
}

//------------------------------------------------------------------------------

template<> inline QString SParameters::valueToStringLabel<double>(double value, std::uint8_t decimals)
{
    return QString::number(value, 'f', decimals);
}

//------------------------------------------------------------------------------

template<typename T>
void SParameters::setLabelMinimumSize(QLabel* label, T min, T max, std::uint8_t decimals)
{
    const auto minString = valueToStringLabel(min, decimals);
    const auto maxString = valueToStringLabel(max, decimals);

    // Create a dummy label with same properties
    QLabel dummyLabel;
    dummyLabel.setFont(label->font());
    dummyLabel.setStyleSheet(label->styleSheet());

    // Fill it with the string of the max value and request the size from Qt
    dummyLabel.setText(maxString);
    const QSize sizeWithMaxValue = dummyLabel.sizeHint();

    // Fill it with the string of the min value and request the size from Qt
    dummyLabel.setText(minString);
    const QSize sizeWithMinValue = dummyLabel.sizeHint();

    // Compute the maximum size and set it to our label
    const QSize maxSize = sizeWithMaxValue.expandedTo(sizeWithMinValue);
    label->setMinimumSize(maxSize);
}

//------------------------------------------------------------------------------

}   //namespace guiQt
}   //namespace editor
