import random
import os

seed_data = os.urandom(16)
seed = int.from_bytes(seed_data, 'big')
random.seed(seed)

query = ""
if "QUERY_STRING" in os.environ:
    query = os.environ["QUERY_STRING"]

value = ""
if query == "roll=on":
    value = f"<br><p>Value of dice roll : {random.randint(1,6)}</p>"
content_length = 0
separator = "\r\n"
body = f"""<html>
    <body>
        <h1>Click here to generate a Random Value</h1>
        <a href=\"/cgi/diceroll.py.cgi?roll=on\"><button>Click Me !</button></a>
        {value}
        <br><br><a href=\"/\">Return Home !</a>
    </body>
</html>
"""

headers = "Content-type: text/html; charset=UTF-8"
headers = f"Content-length: {len(body)}" + separator + headers

if os.getenv("HTTP_CONNECTION") == "keep-alive":
    headers = f"Connection: {os.environ['HTTP_CONNECTION']}" + separator + headers
else:
    headers = "Connection: close" + separator

status = "Status: 200 OK" + separator

print(status + headers + 2 * separator + body)


