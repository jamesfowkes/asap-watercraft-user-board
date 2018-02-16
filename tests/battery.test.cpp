/*
 * C standard library Includes
 */

#include <stdint.h>
#include <stdbool.h>

/*
 * CPPUnit
 */

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

/*
 * Application Includes
 */

#include "../controller.h"
#include "../battery.h"

/*
 * Required Mock Functions
 */

extern void adc_mock_set(ADC_CHANNEL_ENUM adc_input, uint16_t value);

bool in_debug_mode() { return false; }

class BatteryTest : public CppUnit::TestFixture  {

	CPPUNIT_TEST_SUITE(BatteryTest);

	CPPUNIT_TEST(TestHystersisAppliedToVoltageRising);
	CPPUNIT_TEST(TestHystersisAppliedToVoltageFalling);

	CPPUNIT_TEST_SUITE_END();

	void TestHysteresisAppliedRising(uint8_t old_state)
	{
		uint16_t transition_level = LEVELS[old_state+1]+HYSTERESIS[old_state+1];
		adc_mock_set(ADC_CHANNEL_BATTERY_VOLTAGE, transition_level);
		battery_tick(UINT32_MAX);
		CPPUNIT_ASSERT_EQUAL(old_state, battery_get_last_state(ADC_CHANNEL_BATTERY_VOLTAGE));

		adc_mock_set(ADC_CHANNEL_BATTERY_VOLTAGE, transition_level+1);
		battery_tick(UINT32_MAX);
		CPPUNIT_ASSERT_EQUAL(old_state+1, (int)battery_get_last_state(ADC_CHANNEL_BATTERY_VOLTAGE));
	}

	void TestHysteresisAppliedFalling(uint8_t old_state)
	{
		uint16_t transition_level = LEVELS[old_state]-HYSTERESIS[old_state];
		adc_mock_set(ADC_CHANNEL_BATTERY_VOLTAGE, transition_level);
		battery_tick(UINT32_MAX);
		CPPUNIT_ASSERT_EQUAL(old_state, battery_get_last_state(ADC_CHANNEL_BATTERY_VOLTAGE));

		adc_mock_set(ADC_CHANNEL_BATTERY_VOLTAGE, transition_level-1);
		battery_tick(UINT32_MAX);
		CPPUNIT_ASSERT_EQUAL(old_state-1, (int)battery_get_last_state(ADC_CHANNEL_BATTERY_VOLTAGE));
	}

	void TestHystersisAppliedToVoltageRising()
	{
		battery_tick(UINT32_MAX);
		CPPUNIT_ASSERT_EQUAL(0, (int)battery_get_last_state(ADC_CHANNEL_BATTERY_VOLTAGE));

		TestHysteresisAppliedRising(0);
		TestHysteresisAppliedRising(1);
		TestHysteresisAppliedRising(2);
		TestHysteresisAppliedRising(3);
		TestHysteresisAppliedRising(4);
		TestHysteresisAppliedRising(5);
		TestHysteresisAppliedRising(6);
	}

	void TestHystersisAppliedToVoltageFalling()
	{
		adc_mock_set(ADC_CHANNEL_BATTERY_VOLTAGE, LEVELS[7] + HYSTERESIS[7]);
		battery_tick(UINT32_MAX);

		CPPUNIT_ASSERT_EQUAL(7, (int)battery_get_last_state(ADC_CHANNEL_BATTERY_VOLTAGE));

		TestHysteresisAppliedFalling(7);
		TestHysteresisAppliedFalling(6);
		TestHysteresisAppliedFalling(5);
		TestHysteresisAppliedFalling(4);
		TestHysteresisAppliedFalling(3);
		TestHysteresisAppliedFalling(2);
		TestHysteresisAppliedFalling(1);
	}

};

int main()
{
   CppUnit::TextUi::TestRunner runner;
   
   CPPUNIT_TEST_SUITE_REGISTRATION( BatteryTest );

   CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();

   runner.addTest( registry.makeTest() );
   runner.run();

   return 0;
}
