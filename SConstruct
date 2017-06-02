import os

cppdefines = ['F_CPU=1000000UL']
cppflags = ['-Wall', '-Wextra', '-pedantic', '-mmcu=atmega8535', '-g', '-Os', '-ffunction-sections', '-fdata-sections']
linker_flags = ['-Wl,--gc-sections', '-Os', '-mmcu=atmega8535']

env = Environment(ENV=os.environ, CC="avr-gcc", CXX="avr-g++", CPPFLAGS=cppflags, CPPDEFINES=cppdefines)

sources = Glob("*.cpp")

objects = [ env.Object(src) for src in sources ]

elf_builder = env.Program('controller.elf', objects, LINKFLAGS=linker_flags)
hex_copier = env.Command("controller.hex", "controller.elf", "avr-objcopy -R .eeprom -O ihex $SOURCE $TARGET")

env.AlwaysBuild(elf_builder)
env.AlwaysBuild(hex_copier)

if 'upload-arduino' in COMMAND_LINE_TARGETS:
    port = ARGUMENTS["port"]
    upload_command = env.Command(None, "controller.hex", "avrdude -pm8535 -carduino -P%s -Uflash:w:$SOURCE:a" % port)
    env.Alias("upload-arduino", upload_command)

elif 'upload-tiny' in COMMAND_LINE_TARGETS:
    upload_command = env.Command(None, "controller.hex", "avrdude -pm8535 -cusbtiny -Uflash:w:$SOURCE:a")
    env.Alias("upload-tiny", upload_command)

elif 'upload-isp' in COMMAND_LINE_TARGETS:
    upload_command = env.Command(None, "controller.hex", "avrdude -pm8535 -cavrispmkii -Uflash:w:$SOURCE:a")
    env.Alias("upload-isp", upload_command)
