import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';

@Injectable({
  providedIn: 'root'
})
export class DataService {

  constructor(private http: HttpClient) { }

  Salvar(saludador:String[]){
    this.http.post('https://mensajes-68ee3-default-rtdb.firebaseio.com/saludador.json', saludador)
    .subscribe(
      response => { console.log('respuesta del servidor ' + response)},
      error => console.error('error del servidor ' + error)
      );
  }
}
