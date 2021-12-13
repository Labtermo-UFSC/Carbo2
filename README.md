# Carbo2
O carbo 2 é um sistema de monitoramento de concentração de CO2 e permite relacionar os dados com a circulação de ar e assim mostrar o quão apto o ambiente está para ser utilizado na pandemia. 

## Requisitos
- Broker MQTT (Recomendo rodar o Mosquitto em algum server
- Base de dados (Recomendo rodar junto com o broker e em conjunto com algo para visualizar os dados)
- secrets.yaml (Arquivo que recebe as credenciais (pretendo adicionar um modelo no futuro) 

## Instalação
```sh
pip3 install esphome
pip3 install pillow  
esphome run 000x-x.yaml  
```
