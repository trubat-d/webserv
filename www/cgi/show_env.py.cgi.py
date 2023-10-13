import os


def generate_env():
    res = ""
    for e in os.environ:
        res += f"<p>{e}={os.environ[e]}</p>\n"
    return res


content_length = 0
separator = "\r\n"
body = f"""<html>
    <body>
        <h1>Env (Python):</h1>
        {generate_env()}
        <br><br><a href=\"/\">Return Home !</a>
    </body>
</html>
"""

headers = "Content-type: text/html; charset=UTF-8"
headers = f"Content-length: {len(body)}" + separator + headers

if os.environ["HTTP_CONNECTION"] == "keep-alive":
    headers = f"Connection: {os.environ['HTTP_CONNECTION']}" + separator + headers
else:
    headers = "Connection: close" + separator

status = "Status: 200 OK" + separator

print(status + headers + 2 * separator + body)


