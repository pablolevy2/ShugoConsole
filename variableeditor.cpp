#include "variableeditor.h"

/*
 *        VariableEditorBase
 */

VariableEditorBase::VariableEditorBase(QObject* parent,
                      const QString& variableName,
                      const QString& settingNameCheck,
                      const QString& settingNameValue,
                      QCheckBox* checkBox)

    : QObject(parent),
      _variableName(variableName),
      _settingNameCheck(settingNameCheck),
      _settingNameValue(settingNameValue),
      _checkBox(checkBox)
{
}

void VariableEditorBase::connectInterface()
{
    connect(_checkBox, &QCheckBox::stateChanged, this, &VariableEditorBase::onCheckStateChanged);
}

void VariableEditorBase::loadSettings(QSettings& settings)
{
    _checkBox->setChecked(settings.value(_settingNameCheck, QVariant(false)).toBool());
    if(_checkBox->isChecked())
        setValue();
}

void VariableEditorBase::saveSettings(QSettings& settings)
{
    settings.setValue(_settingNameCheck, _checkBox->isChecked());
}

void VariableEditorBase::onCheckStateChanged(int)
{
    updateVar();
}


void VariableEditorBase::updateVar()
{
    if(_checkBox->isChecked())
    {
        setValue();
    }
    else
    {
        emit varDisable(_variableName);
    }

    emit settingsUpdated();
}

/*
 *        DoubleSpinVariableEditor
 */

DoubleSpinVariableEditor::DoubleSpinVariableEditor(QObject* parent,
                 const QString& variableName,
                 const QString& settingNameCheck,
                 const QString& settingNameValue,
                 QCheckBox* checkBox,
                 QDoubleSpinBox* doubleSpinBox,
                 double defaultValue,
                 double minValue,
                 double maxValue)

    : VariableEditorBase(parent, variableName, settingNameCheck, settingNameValue, checkBox),
      _doubleSpinBox(doubleSpinBox),
      _defaultValue(defaultValue),
      _minValue(minValue),
      _maxValue(maxValue)
{
}

void DoubleSpinVariableEditor::connectInterface()
{
    VariableEditorBase::connectInterface();
    connect(_doubleSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &DoubleSpinVariableEditor::onDoubleSpinBoxValueChanged);
}

void DoubleSpinVariableEditor::loadSettings(QSettings& settings)
{
    QVariant vValue = settings.value(_settingNameValue, _defaultValue);
    double Value = vValue.toDouble();
    if(Value < _minValue || Value > _maxValue)
        Value = _defaultValue;

    _doubleSpinBox->setValue(Value);

    VariableEditorBase::loadSettings(settings);
}

void DoubleSpinVariableEditor::saveSettings(QSettings& settings)
{
    settings.setValue(_settingNameValue, _doubleSpinBox->value());
    VariableEditorBase::saveSettings(settings);
}

void DoubleSpinVariableEditor::setValue()
{
    emit varEnable(_variableName, _doubleSpinBox->value());
}

void DoubleSpinVariableEditor::resetDefaultValue()
{
    _checkBox->setChecked(false);
    _doubleSpinBox->setValue(_defaultValue);
    updateVar();
}

void DoubleSpinVariableEditor::onDoubleSpinBoxValueChanged(double)
{
    updateVar();
}

/*
 *        BoolComboVariableEditor
 */

#define COMBO_INDEX_TRUE 0
#define COMBO_INDEX_FALSE 1

BoolComboVariableEditor::BoolComboVariableEditor(QObject* parent,
                 const QString& variableName,
                 const QString& settingNameCheck,
                 const QString& settingNameValue,
                 QCheckBox* checkBox,
                 QComboBox* comboBox,
                 bool defaultValue,
                 QVariant valueTrue,
                 QVariant valueFalse)

    : VariableEditorBase(parent, variableName, settingNameCheck, settingNameValue, checkBox),
      _comboBox(comboBox),
      _defaultValue(defaultValue),
      _valueTrue(valueTrue),
      _valueFalse(valueFalse)
{
}

void BoolComboVariableEditor::connectInterface()
{
    VariableEditorBase::connectInterface();
    connect(_comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &BoolComboVariableEditor::onComboBoxCurrentIndexChanged);
}

void BoolComboVariableEditor::loadSettings(QSettings& settings)
{
    QVariant vValue = settings.value(_settingNameValue, _defaultValue);

    _comboBox->setCurrentIndex(vValue.toBool() ? COMBO_INDEX_TRUE : COMBO_INDEX_FALSE);

    VariableEditorBase::loadSettings(settings);
}

void BoolComboVariableEditor::saveSettings(QSettings& settings)
{
    bool value = _comboBox->currentIndex() == COMBO_INDEX_TRUE;
    settings.setValue(_settingNameValue, value);
    VariableEditorBase::saveSettings(settings);
}

void BoolComboVariableEditor::setValue()
{
    bool value = _comboBox->currentIndex() == COMBO_INDEX_TRUE;
    emit varEnable(_variableName, value ? _valueTrue : _valueFalse);
}

void BoolComboVariableEditor::resetDefaultValue()
{
    _checkBox->setChecked(false);
    _comboBox->setCurrentIndex(_defaultValue ? COMBO_INDEX_TRUE : COMBO_INDEX_FALSE);
    updateVar();
}


void BoolComboVariableEditor::onComboBoxCurrentIndexChanged(int)
{
    updateVar();
}

/*
 *        ValueComboVariableEditor
 */


ValueComboVariableEditor::ValueComboVariableEditor(QObject* parent,
                 const QString& variableName,
                 const QString& settingNameCheck,
                 const QString& settingNameValue,
                 QCheckBox* checkBox,
                 QComboBox* comboBox,
                 int defaultValueIndex)

    : VariableEditorBase(parent, variableName, settingNameCheck, settingNameValue, checkBox),
      _comboBox(comboBox),
      _defaultValueIndex(defaultValueIndex)
{
}

void ValueComboVariableEditor::connectInterface()
{
    VariableEditorBase::connectInterface();
    connect(_comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ValueComboVariableEditor::onComboBoxCurrentIndexChanged);
}

void ValueComboVariableEditor::loadSettings(QSettings& settings)
{
    QVariant vValue = settings.value(_settingNameValue, _comboBox->itemText(_defaultValueIndex));

    int i = _comboBox->findText(vValue.toString());
    if(i != -1)
    {
        _comboBox->setCurrentIndex(i);
    }
    else
    {
        _comboBox->setCurrentIndex(_defaultValueIndex);
    }

    VariableEditorBase::loadSettings(settings);
}

void ValueComboVariableEditor::saveSettings(QSettings& settings)
{
    settings.setValue(_settingNameValue, _comboBox->currentText());
    VariableEditorBase::saveSettings(settings);
}

void ValueComboVariableEditor::setValue()
{
    double value = _comboBox->currentText().toDouble();
    emit varEnable(_variableName, value);
}

void ValueComboVariableEditor::resetDefaultValue()
{
   _checkBox->setChecked(false);
   _comboBox->setCurrentIndex(_defaultValueIndex);
   updateVar();
}

void ValueComboVariableEditor::onComboBoxCurrentIndexChanged(int)
{
    updateVar();
}

