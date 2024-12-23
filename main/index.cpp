const char* html_content = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>智能健康</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
            background: linear-gradient(to bottom, #ff9a9e, #fad0c4, #fad390);
            color: #333;
        }

        header {
            text-align: center;
            background-color: #4CAF50;
            color: white;
            padding: 20px 0;
        }

        .container {
            display: flex;
            justify-content: center;
            flex-wrap: wrap;
            gap: 20px;
            padding: 20px;
        }

        .card {
            background: white;
            box-shadow: 0px 4px 8px rgba(0, 0, 0, 0.2);
            border-radius: 10px;
            width: 200px;
            text-align: center;
            padding: 20px;
            font-size: 1.2em;
            transition: transform 0.3s ease;
        }

        .card:hover {
            transform: scale(1.1);
        }

        .card h2 {
            margin: 10px 0;
            color: #17d020;
        }

        .card .value {
            font-size: 2em;
            color: #ff5722;
        }
    </style>
</head>
<body>
    <header>
        <h1>智能健康医疗系统</h1>
    </header>

    <div class="container">
        <div class="card" id="temperature">
            <h2>环境温度</h2>
            <div class="value" id="tempValue">%.1f</div>
        </div>
        <div class="card" id="bodyTemperature">
            <h2>体温</h2>
            <div class="value" id="bodyTempValue">%.1f</div>
        </div>
        <div class="card" id="heartRate">
            <h2>心率</h2>
            <div class="value" id="heartRateValue">%.1f</div>
        </div>
        <div class="card" id="oxygen">
            <h2>血氧饱和度</h2>
            <div class="value" id="oxygenValue">%.1f</div>
        </div>
    </div>

    <script>
       // setInterval(fetchHealthData, 3000);
    </script>
</body>
</html>
)";
