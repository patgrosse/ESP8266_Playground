Import("env")

defines = []
with open(".defines.ini") as f:
    content = f.readlines()
    for line in content:
        parts = line.rstrip().split("=", 1)
        defines.append((parts[0], parts[1]))
env.Append(CPPDEFINES=defines)
