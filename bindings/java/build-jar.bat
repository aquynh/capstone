set mvnErr=0

call mvn clean package -f capstone/pom.xml || set mvnErr=1

if "%mvnErr%" == "1" (
    exit 1
) else (
    exit 0
)