import random
import os

seed_data = os.urandom(16)
seed = int.from_bytes(seed_data, 'big')
random.seed(seed)

content_length = 0
separator = "\r\n"
body = f"""<html>
    <body>
        <h1>Here is the seed : {seed}</h1>
    </body>
</html>
"""

headers = "Content-type: text/html; charset=UTF-8"
headers = f"Content-length: {len(body)}" + separator + headers

status = "Status: 200 OK" + separator

print(status + headers + 2 * separator + body)


