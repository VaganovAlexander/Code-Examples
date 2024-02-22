FROM ubuntu:latest

RUN apt-get update -y && apt-get install -y g++ libsfml-dev cmake xauth

COPY . .

EXPOSE 8878

RUN chmod +x project_builder.sh

CMD ./project_builder.sh