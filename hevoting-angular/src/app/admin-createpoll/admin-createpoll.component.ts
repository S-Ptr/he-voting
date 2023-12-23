import { Component, OnInit } from '@angular/core';
import { VotingService } from '../voting.service';
import { ReactiveFormsModule, FormGroup, FormBuilder, FormArray } from '@angular/forms';
import { Route, Router } from '@angular/router';

@Component({
  selector: 'admin-createpoll',
  templateUrl: './admin-createpoll.component.html',
  styleUrls: ['./admin-createpoll.component.css']
})
export class AdminCreatepollComponent implements OnInit {

  constructor(private votingservice:VotingService, private formBuilder:FormBuilder, private router:Router) { }
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
    this.votingservice.createPoll(value).subscribe((data)=>{
      console.log(data)
    });
  }

  reset(): void {
    this.form.reset();
    this.contacts.clear();
    this.addContactField();
  }

}
