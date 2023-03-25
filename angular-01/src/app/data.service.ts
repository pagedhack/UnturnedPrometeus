import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { map } from 'rxjs/operators';
import { Observable } from 'rxjs';

@Injectable({
  providedIn: 'root'
})
export class DataService {
  items: any[] = [];

  constructor(private http: HttpClient) { }

  Salvar(saludador:String[]){
    this.http.post('https://mensajes-68ee3-default-rtdb.firebaseio.com/saludador.json', saludador)
    .subscribe(
      response => { console.log('respuesta del servidor ' + response)},
      error => console.error('error del servidor ' + error)
      );
  }

  mostrarSaludador(): Observable<any[]>{
    return this.http.get('https://mensajes-68ee3-default-rtdb.firebaseio.com/saludador.json').pipe(
        map( (responseData: any) => {
          const items = [];
          for (const key in responseData){
            if (responseData.hasOwnProperty(key)) {
              items.push({
                id: key, ...responseData[key]
              });              
            }
          }
          return items;
        }))
  }

  //viejo
  mostrarSaludadorConsola(saludador:String[]){
    this.http.get('https://mensajes-68ee3-default-rtdb.firebaseio.com/saludador.json').pipe(
        map( (responseData: any) => {
          const items = [];
          for (const key in responseData){
            if (responseData.hasOwnProperty(key)) {
              items.push({
                id: key, ...responseData[key]
              });              
            }
          }
          return items;
        }))
    .subscribe(
      items => {console.log(items);
      }
      );
  }
}
