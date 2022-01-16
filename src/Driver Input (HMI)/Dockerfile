FROM python:3

RUN pip install --no-cache-dir rticonnextdds_connector numpy
COPY . /app
WORKDIR /app

CMD python main.py 
