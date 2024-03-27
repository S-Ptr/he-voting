import { Component, OnInit } from '@angular/core';
import { VotingService } from '../voting.service';
import { ReactiveFormsModule, FormGroup, FormBuilder, FormArray} from '@angular/forms';
import { Route, Router } from '@angular/router';
import { MatSnackBar } from '@angular/material/snack-bar';

@Component({
  selector: 'admin-createpoll',
  templateUrl: './admin-createpoll.component.html',
  styleUrls: ['./admin-createpoll.component.css']
})
export class AdminCreatepollComponent implements OnInit {

  startTime:any;
  endTime:any;
  msg:string = "";

  constructor(private votingservice:VotingService, private formBuilder:FormBuilder, private router:Router, private snackbar: MatSnackBar) { }

  openSnackBar(message: string, action: string) {
    this.snackbar.open(message, action, {duration:3000});
  }

  ngOnInit(): void {
    
  }

  checkConfirm = false;
  contact = {
    title: 'Tema',
    contacts: [{ name: 'Kandidat'}]
  }

  form: FormGroup = this.formBuilder.group({
    title: this.contact.title,
    contacts: this.buildContacts(this.contact.contacts)
  });

  get contacts(): FormArray {
    return this.form.get('contacts') as FormArray;
  }

  buildContacts(contacts: {name: string;}[] = []) {
    return this.formBuilder.array(contacts.map(contact => this.formBuilder.group(contact)));
  }

  addContactField() {
    this.contacts.push(this.formBuilder.group({name: null}))
  }

  removeContactField(index: number): void {
    if (this.contacts.length > 1) this.contacts.removeAt(index);
    else this.contacts.patchValue([{name: null}]);
  }

  submit(value: any): void {
    if(this.contact.title == ""){
      this.openSnackBar("Unesite naslov", "X");
      return;
    }
    for(let i = 0; i < value.contacts.length;i++){
      if(value.contacts[i].name == null){
        this.openSnackBar("Unesite naziv svih kandidata", "X");
        return;
      }
    }
    if(this.startTime == null || this.endTime == null){
      this.openSnackBar("Popunite polja za vreme", "X");
      return;
    }
    let startTimeDate = Date.parse(this.startTime);
    let endTimeDate = Date.parse(this.endTime);
    if(endTimeDate <= startTimeDate){
      this.openSnackBar("Vremena su nepravilna", "X")
      return;
    }
    value.startTime = startTimeDate;
    value.endTime = endTimeDate;

    this.votingservice.createPoll(value).subscribe((data)=>{
      console.log(data)
      this.openSnackBar("Izbor je raspisan", "OK")
    });
  }

  reset(): void {
    this.form.reset();
    this.contacts.clear();
    this.addContactField();
  }

}
