import { Component } from '@angular/core';
import { DataService } from '../data.service';

@Component({
  selector: 'app-datos',
  templateUrl: './datos.component.html',
  styleUrls: ['./datos.component.css']
})
export class DatosComponent {

  constructor(private dataService: DataService) {}

  ngOnInit(){
    this.dataService.Salvar(["Jorge"]);
  }
}
