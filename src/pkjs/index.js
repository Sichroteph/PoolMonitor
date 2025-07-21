var API_URL = "https://api.iopool.com/v1/pools";


function sendPoolDataToPebble(nTemp, nPH, nORP) {
  //poolTemp = Math.round(28 * 100); // Assurez-vous que c'est un entier
  //poolPH = Math.round(7.2 * 100);  // Assurez-vous que c'est un entier
  //poolORP = 654;                   // Déjà un entier
  var units = 0;
  units = parseInt(localStorage.getItem(10));
  Pebble.sendAppMessage({
    "KEY_TEMP": nTemp,
    "KEY_PH": nPH,
    "KEY_ORP": nORP,
    "KEY_RADIO_UNITS": units
  }, function () {
    console.log("Données envoyées avec succès à Pebble");

  }, function (e) {
    console.error("Erreur lors de l'envoi des données à Pebble: " + e.error.message);
  });
}

Pebble.addEventListener('appmessage', function (e) {
  console.log("Message reçu de la montre : ", e.payload);
  getIOPoolData();
});

function getIOPoolData() {
  console.log("getIOPoolData");
sendPoolDataToPebble(280, 734, 678);
  var apiKey = localStorage.getItem(11);


  // pour test
  

  if (apiKey !== null) {

    var size = apiKey.length;

    var poolTemp;
    var poolPH;
    var poolORP;
    if (size == 40) {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", API_URL, true);
      xhr.setRequestHeader("x-api-key", apiKey);
      xhr.onload = function () {
        console.log(xhr.status);

        if (xhr.status === 200) {
          try {
            var data = JSON.parse(xhr.responseText);
            if (data && data.length > 0 && data[0].latestMeasure) {
              poolTemp = Math.round(data[0].latestMeasure.temperature * 10);
              poolPH = Math.round(data[0].latestMeasure.ph * 100);
              poolORP = Math.round(data[0].latestMeasure.orp);
              console.log("poolTemp");
              console.log(poolTemp);
              console.log("poolPH");
              console.log(poolPH);
              console.log("poolORP");
              console.log(poolORP);

              sendPoolDataToPebble(poolTemp, poolPH, poolORP);

            } else {
              console.error("Données inattendues :", data);
            }
          } catch (e) {
            console.error("Erreur de parsing JSON :", e);
          }
        } else {
          console.error("Erreur HTTP :", xhr.status, xhr.statusText);
        }
      };
      xhr.onerror = function () {
        console.error("Erreur réseau");
      };
      xhr.send();

    }
    else {
      console.error("API key manquante");
    }

  } else {
    console.error("API key manquante");
  }
}
Pebble.addEventListener('showConfiguration', function () {
  //var url = 'http://sichroteph.github.io/Ruler-Weather/';
  var url = 'http://sichroteph.github.io/PoolMonitor/';

  Pebble.openURL(url);
});


Pebble.addEventListener('webviewclosed', function (e) {
  var configData = JSON.parse(decodeURIComponent(e.response));
  console.log('Configuration page returned: ' + JSON.stringify(configData));



  var input_iopool_token = configData['input_iopool_token'];
  var radio_units = 0;
  radio_units = configData['radio_units'] ? 1 : 0;


  var dict = {};
  console.log('radio units: ' + radio_units);
  localStorage.setItem(10, radio_units ? 1 : 0);
  localStorage.setItem(11, configData['input_iopool_token']);

  dict['KEY_RADIO_UNITS'] = radio_units;

  getIOPoolData();

});


Pebble.addEventListener('ready', function () {
  console.log("PebbleKit JS prêt !");
  getIOPoolData();
});