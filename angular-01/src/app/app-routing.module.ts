import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { Routes, RouterModule } from '@angular/router';
import { InicioComponent } from './inicio/inicio.component';
import { AcercaComponent } from './acerca/acerca.component';
import { ContactoComponent } from './contacto/contacto.component';
import { DatosComponent } from './datos/datos.component';
import { FormularioComponent } from './formulario/formulario.component';
import { MostrarComponent } from './mostrar/mostrar.component';


const rutas: Routes = [
  {path: 'inicio', component : InicioComponent},
  {path: 'acerca', component : AcercaComponent},
  {path: 'contacto', component : ContactoComponent},
  {path: 'Datos', component: DatosComponent},
  {path: 'Formulario', component: FormularioComponent},
  {path: 'Mostrar', component: MostrarComponent}
]

@NgModule({
  declarations: [],
  imports: [
    CommonModule, RouterModule.forRoot(rutas)
  ],
  exports: [RouterModule] 
})
export class AppRoutingModule { }
