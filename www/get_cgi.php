<!DOCTYPE html>
<html>
<body>

<h1>My first PHP page</h1>

<?php
echo "the environment variables : <br>";

echo "<br> 0: REQUEST_METHOD = ";
echo $_SERVER['REQUEST_METHOD'];
echo "<br><br> 1: CONTENT_LENGTH = ";
echo $_SERVER['CONTENT_LENGTH'];
echo "<br><br> 2: CONTENT_TYPE = ";
echo $_SERVER['CONTENT_TYPE'];
echo "<br><br> 3 : GATEWAY_INTERFACE = ";
echo $_SERVER['GATEWAY_INTERFACE'];
echo "<br><br> 4 : SERVER_NAME = ";
echo $_SERVER['SERVER_NAME'];
echo "<br><br> 5 : SERVER_PORT = ";
echo $_SERVER['SERVER_PORT'];
echo "<br><br> 6 : SERVER_PROTOCOL = ";
echo $_SERVER['SERVER_PROTOCOL'];
echo "<br><br> 7 : SERVER_SOFTWARE = ";
echo $_SERVER['SERVER_SOFTWARE'];
echo "<br><br> 8 : QUERY_STRING = ";
echo $_SERVER['QUERY_STRING'];
?>

</body>
</html>