import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';

import
{
  Environment
}
from '../../environments/environment';

import
{
  ICredentials,
}
from '../models/credentials';

import
{
  User
}
from '../models/user';

import
{
  Service
}
from './service';

@Injectable({
  providedIn: 'root'
})

export class LoginService
{
  constructor(
    private environment: Environment,
    private service: Service,
  )
  {
    service.service_uri = environment.api_uri();
  }

  do(credentials:ICredentials):Observable<User>
  {
    return this.service.post<User>(
      credentials,
      'login', '',
      'Logging in...',
      'Login failed!', new User(),
      'Logged in!',
    );
  }
}
