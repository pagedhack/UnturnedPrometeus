import { Component } from '@angular/core';
import { DataService } from '../data.service';

@Component({
  selector: 'app-formulario',
  templateUrl: './formulario.component.html',
  styleUrls: ['./formulario.component.css']
})
export class FormularioComponent {

  nombre:string = '';

  constructor(private dataService: DataService) { }

  onSubmit(){
    this.dataService.Salvar([this.nombre]);
  }
}
